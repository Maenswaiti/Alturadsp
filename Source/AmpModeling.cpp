#include "AmpModeling.h"

AmpModeling::AmpModeling() : oversampling(2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR)
{
    gainSmoothed.setCurrentAndTargetValue(0.5f);
    bassSmoothed.setCurrentAndTargetValue(0.5f);
    midSmoothed.setCurrentAndTargetValue(0.5f);
    trebleSmoothed.setCurrentAndTargetValue(0.5f);
    presenceSmoothed.setCurrentAndTargetValue(0.5f);
}

AmpModeling::~AmpModeling()
{
}

void AmpModeling::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    
    oversampling.initProcessing(spec.maximumBlockSize);
    
    auto oversampledSpec = spec;
    oversampledSpec.sampleRate *= oversampling.getOversamplingFactor();
    
    lowShelf.prepare(oversampledSpec);
    midPeak.prepare(oversampledSpec);
    highShelf.prepare(oversampledSpec);
    presenceFilter.prepare(oversampledSpec);
    inputGain.prepare(oversampledSpec);
    outputGain.prepare(oversampledSpec);
    ladderFilter.prepare(oversampledSpec);
    
    gainSmoothed.reset(spec.sampleRate, 0.05);
    bassSmoothed.reset(spec.sampleRate, 0.05);
    midSmoothed.reset(spec.sampleRate, 0.05);
    trebleSmoothed.reset(spec.sampleRate, 0.05);
    presenceSmoothed.reset(spec.sampleRate, 0.05);
    
    updateFilters();
    updateDistortion();
}

void AmpModeling::process(juce::dsp::ProcessContextReplacing<float>& context)
{
    auto oversampledBlock = oversampling.processSamplesUp(context.getInputBlock());
    juce::dsp::ProcessContextReplacing<float> oversampledContext(oversampledBlock);
    
    inputGain.process(oversampledContext);
    lowShelf.process(oversampledContext);
    midPeak.process(oversampledContext);
    highShelf.process(oversampledContext);
    
    for (size_t channel = 0; channel < oversampledBlock.getNumChannels(); ++channel)
    {
        auto* channelData = oversampledBlock.getChannelPointer(channel);
        
        for (size_t sample = 0; sample < oversampledBlock.getNumSamples(); ++sample)
        {
            float input = channelData[sample];
            float output = input;
            
            float currentGain = gainSmoothed.getNextValue();
            float drive = 1.0f + currentGain * 5.0f;
            
            switch (currentModel)
            {
                case Clean:
                    output = advancedTubeWaveShaper(input * drive * 0.3f, 0.1f);
                    break;
                case Crunch:
                    output = asymmetricTubeWaveShaper(input * drive * 0.8f, 0.3f);
                    output = dynamicTubeCompression(output, 0.2f);
                    break;
                case Lead:
                    output = volterraSeriesWaveShaper(input * drive * 1.2f, 0.4f);
                    output = dynamicTubeCompression(output, 0.4f);
                    break;
                case HighGain:
                    output = asymmetricTubeWaveShaper(input * drive * 1.8f, 0.6f);
                    output = volterraSeriesWaveShaper(output, 0.3f);
                    output = dynamicTubeCompression(output, 0.6f);
                    break;
                case Vintage:
                    output = advancedTubeWaveShaper(input * drive * 0.6f, 0.8f);
                    output = dynamicTubeCompression(output, 0.3f);
                    tubeTemperature = juce::jlimit(0.5f, 1.5f, tubeTemperature + (std::abs(output) - 0.5f) * 0.001f);
                    output *= tubeTemperature;
                    break;
                case Modern:
                    output = volterraSeriesWaveShaper(input * drive * 1.5f, 0.5f);
                    output = asymmetricTubeWaveShaper(output, 0.4f);
                    output = dynamicTubeCompression(output, 0.5f);
                    break;
            }
            
            channelData[sample] = output;
            previousSample = output;
        }
    }
    
    ladderFilter.process(oversampledContext);
    presenceFilter.process(oversampledContext);
    outputGain.process(oversampledContext);
    
    oversampling.processSamplesDown(context.getOutputBlock());
}

void AmpModeling::reset()
{
    lowShelf.reset();
    midPeak.reset();
    highShelf.reset();
    presenceFilter.reset();
    inputGain.reset();
    outputGain.reset();
}

void AmpModeling::setInstrumentType(bool isBass)
{
    isBassMode = isBass;
    updateFilters();
}

void AmpModeling::setAmpModel(AmpModel model)
{
    currentModel = model;
    updateDistortion();
    updateFilters();
}

void AmpModeling::setGain(float gain)
{
    float clampedGain = juce::jlimit(0.0f, 10.0f, gain);
    inputGain.setGainDecibels((clampedGain - 5.0f) * 2.4f);
    gainSmoothed.setTargetValue(clampedGain / 10.0f);
}

void AmpModeling::setBass(float bass)
{
    float clampedBass = juce::jlimit(0.0f, 1.0f, bass);
    bassSmoothed.setTargetValue(clampedBass);
    updateFilters();
}

void AmpModeling::setMid(float mid)
{
    float clampedMid = juce::jlimit(0.0f, 1.0f, mid);
    midSmoothed.setTargetValue(clampedMid);
    updateFilters();
}

void AmpModeling::setTreble(float treble)
{
    float clampedTreble = juce::jlimit(0.0f, 1.0f, treble);
    trebleSmoothed.setTargetValue(clampedTreble);
    updateFilters();
}

void AmpModeling::setPresence(float presence)
{
    float clampedPresence = juce::jlimit(0.0f, 1.0f, presence);
    presenceSmoothed.setTargetValue(clampedPresence);
    updateFilters();
}

void AmpModeling::updateFilters()
{
    double oversampledSampleRate = sampleRate * oversampling.getOversamplingFactor();
    
    float bassFreq = isBassMode ? 60.0f : 80.0f;
    float midFreq = isBassMode ? 400.0f : 600.0f;
    float trebleFreq = isBassMode ? 2500.0f : 4000.0f;
    float presenceFreq = isBassMode ? 1800.0f : 3500.0f;
    
    float bassValue = bassSmoothed.getCurrentValue();
    float midValue = midSmoothed.getCurrentValue();
    float trebleValue = trebleSmoothed.getCurrentValue();
    float presenceValue = presenceSmoothed.getCurrentValue();
    
    float bassGain = (bassValue - 0.5f) * 18.0f;
    float midGain = (midValue - 0.5f) * 15.0f;
    float trebleGain = (trebleValue - 0.5f) * 20.0f;
    float presenceGain = (presenceValue - 0.5f) * 12.0f;
    
    *lowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(oversampledSampleRate, bassFreq, 0.8f, juce::Decibels::decibelsToGain(bassGain));
    *midPeak.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(oversampledSampleRate, midFreq, 1.2f, juce::Decibels::decibelsToGain(midGain));
    *highShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(oversampledSampleRate, trebleFreq, 0.8f, juce::Decibels::decibelsToGain(trebleGain));
    *presenceFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(oversampledSampleRate, presenceFreq, 1.0f, juce::Decibels::decibelsToGain(presenceGain));
    
    ladderFilter.setCutoffFrequencyHz(trebleFreq + (trebleValue - 0.5f) * 2000.0f);
    ladderFilter.setResonance(0.2f + (presenceValue - 0.5f) * 0.3f);
}

void AmpModeling::updateDistortion()
{
    switch (currentModel)
    {
        case Clean:
            outputGain.setGainDecibels(0.0f);
            break;
        case Crunch:
            outputGain.setGainDecibels(-6.0f);
            break;
        case Lead:
            outputGain.setGainDecibels(-3.0f);
            break;
        case HighGain:
            outputGain.setGainDecibels(-9.0f);
            break;
        case Vintage:
            outputGain.setGainDecibels(-6.0f);
            break;
        case Modern:
            outputGain.setGainDecibels(-6.0f);
            break;
    }
}

float AmpModeling::tubeWaveShaper(float input)
{
    float x = input * 2.0f;
    if (x > 1.0f)
        return (2.0f - (2.0f - x) * (2.0f - x)) / 3.0f;
    else if (x < -1.0f)
        return -(2.0f - (2.0f + x) * (2.0f + x)) / 3.0f;
    else
        return x - x * x * x / 3.0f;
}

float AmpModeling::transistorWaveShaper(float input)
{
    return std::tanh(input * 3.0f) * 0.7f;
}

float AmpModeling::modernWaveShaper(float input)
{
    float x = input * 4.0f;
    return std::tanh(x) * (1.0f - std::exp(-std::abs(x))) * 0.6f;
}

float AmpModeling::advancedTubeWaveShaper(float input, float drive)
{
    float x = input * (1.0f + drive * 3.0f);
    float sign = (x >= 0.0f) ? 1.0f : -1.0f;
    float abs_x = std::abs(x);
    
    float alpha = 1.2f + drive * 0.8f;
    float beta = 0.7f + drive * 0.3f;
    
    return sign * (1.0f - std::exp(-std::pow(abs_x, alpha))) * beta;
}

float AmpModeling::asymmetricTubeWaveShaper(float input, float asymmetry)
{
    float x = input;
    
    if (x >= 0.0f)
    {
        return std::tanh(x * (2.0f + asymmetry)) * (0.7f + asymmetry * 0.2f);
    }
    else
    {
        return std::tanh(x * (2.0f - asymmetry * 0.5f)) * (0.7f - asymmetry * 0.1f);
    }
}

float AmpModeling::volterraSeriesWaveShaper(float input, float harmonics)
{
    float x = input;
    float x2 = x * x;
    float x3 = x2 * x;
    
    float fundamental = x;
    float secondHarmonic = harmonics * 0.3f * x2 * (x >= 0.0f ? 1.0f : -1.0f);
    float thirdHarmonic = harmonics * 0.2f * x3;
    
    return fundamental + secondHarmonic + thirdHarmonic;
}

float AmpModeling::dynamicTubeCompression(float input, float compression)
{
    float threshold = 0.6f;
    float ratio = 1.0f + compression * 4.0f;
    
    float abs_input = std::abs(input);
    
    if (abs_input > threshold)
    {
        float excess = abs_input - threshold;
        float compressed_excess = excess / ratio;
        float output_level = threshold + compressed_excess;
        
        return (input >= 0.0f ? 1.0f : -1.0f) * output_level;
    }
    
    return input;
}
