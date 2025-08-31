#include "PluginProcessor.h"
#include "PluginEditor.h"

AlturadspAudioProcessor::AlturadspAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      valueTreeState (*this, nullptr, "Parameters", createParameterLayout()),
      presetManager(valueTreeState)
{
    ampModeling = std::make_unique<AmpModeling>();
    cabSimulation = std::make_unique<CabSimulation>();
    effectsChain = std::make_unique<EffectsChain>();
}

AlturadspAudioProcessor::~AlturadspAudioProcessor()
{
}

const juce::String AlturadspAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AlturadspAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AlturadspAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AlturadspAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AlturadspAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AlturadspAudioProcessor::getNumPrograms()
{
    return 1;
}

int AlturadspAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AlturadspAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AlturadspAudioProcessor::getProgramName (int index)
{
    return {};
}

void AlturadspAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void AlturadspAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    ampModeling->prepare(spec);
    cabSimulation->prepare(spec);
    effectsChain->prepare(spec);
}

void AlturadspAudioProcessor::releaseResources()
{
    ampModeling->reset();
    cabSimulation->reset();
    effectsChain->reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AlturadspAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    #endif

    return true;
  #endif
}
#endif

void AlturadspAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);

    effectsChain->processPreAmp(context);
    ampModeling->process(context);
    cabSimulation->process(context);
    effectsChain->processPostAmp(context);
}

bool AlturadspAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AlturadspAudioProcessor::createEditor()
{
    return new AlturadspAudioProcessorEditor (*this);
}

void AlturadspAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = valueTreeState.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AlturadspAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (valueTreeState.state.getType()))
            valueTreeState.replaceState (juce::ValueTree::fromXml (*xmlState));
}

void AlturadspAudioProcessor::setInstrumentType(InstrumentType type)
{
    currentInstrumentType = type;
    ampModeling->setInstrumentType(type == Bass);
    cabSimulation->setInstrumentType(type == Bass);
    effectsChain->setInstrumentType(type == Bass);
}

juce::AudioProcessorValueTreeState::ParameterLayout AlturadspAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("inputGain", "Input Gain", 
        juce::NormalisableRange<float>(-20.0f, 20.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("outputGain", "Output Gain", 
        juce::NormalisableRange<float>(-20.0f, 20.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>("ampModel", "Amp Model",
        juce::StringArray{"Clean", "Crunch", "Lead", "High Gain", "Vintage", "Modern"}, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("ampGain", "Amp Gain", 
        juce::NormalisableRange<float>(0.0f, 10.0f), 5.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("ampBass", "Bass", 
        juce::NormalisableRange<float>(0.0f, 10.0f), 5.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("ampMid", "Mid", 
        juce::NormalisableRange<float>(0.0f, 10.0f), 5.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("ampTreble", "Treble", 
        juce::NormalisableRange<float>(0.0f, 10.0f), 5.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("ampPresence", "Presence", 
        juce::NormalisableRange<float>(0.0f, 10.0f), 5.0f));

    layout.add(std::make_unique<juce::AudioParameterChoice>("cabModel", "Cab Model",
        juce::StringArray{"4x12 Vintage", "4x12 Modern", "2x12 Combo", "1x12 Studio", "Bass 8x10", "Bass 4x10"}, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("micType", "Mic Type",
        juce::StringArray{"Dynamic 57", "Dynamic 421", "Condenser 414", "Ribbon 121"}, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("micDistance", "Mic Distance", 
        juce::NormalisableRange<float>(0.0f, 10.0f), 2.0f));

    layout.add(std::make_unique<juce::AudioParameterBool>("noiseGateEnable", "Noise Gate", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>("noiseGateThreshold", "Gate Threshold", 
        juce::NormalisableRange<float>(-60.0f, 0.0f), -40.0f));
    
    layout.add(std::make_unique<juce::AudioParameterBool>("compressorEnable", "Compressor", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>("compressorRatio", "Comp Ratio", 
        juce::NormalisableRange<float>(1.0f, 20.0f), 4.0f));
    
    layout.add(std::make_unique<juce::AudioParameterBool>("reverbEnable", "Reverb", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>("reverbSize", "Reverb Size", 
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("reverbDamping", "Reverb Damping", 
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterBool>("delayEnable", "Delay", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>("delayTime", "Delay Time", 
        juce::NormalisableRange<float>(0.0f, 2000.0f), 250.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("delayFeedback", "Delay Feedback", 
        juce::NormalisableRange<float>(0.0f, 0.95f), 0.3f));

    return layout;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AlturadspAudioProcessor();
}
