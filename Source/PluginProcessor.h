#pragma once

#include <JuceHeader.h>
#include "AmpModeling.h"
#include "CabSimulation.h"
#include "EffectsChain.h"
#include "PresetManager.h"

class AlturadspAudioProcessor : public juce::AudioProcessor
{
public:
    AlturadspAudioProcessor();
    ~AlturadspAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getValueTreeState() { return valueTreeState; }
    PresetManager& getPresetManager() { return presetManager; }

    enum InstrumentType
    {
        Guitar = 0,
        Bass = 1
    };

    void setInstrumentType(InstrumentType type);
    InstrumentType getInstrumentType() const { return currentInstrumentType; }

private:
    juce::AudioProcessorValueTreeState valueTreeState;
    
    std::unique_ptr<AmpModeling> ampModeling;
    std::unique_ptr<CabSimulation> cabSimulation;
    std::unique_ptr<EffectsChain> effectsChain;
    PresetManager presetManager;
    
    InstrumentType currentInstrumentType = Guitar;
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AlturadspAudioProcessor)
};
