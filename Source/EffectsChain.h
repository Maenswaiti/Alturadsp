#pragma once

#include <JuceHeader.h>

class EffectsChain
{
public:
    EffectsChain();
    ~EffectsChain();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void processPreAmp(juce::dsp::ProcessContextReplacing<float>& context);
    void processPostAmp(juce::dsp::ProcessContextReplacing<float>& context);
    void reset();
    
    void setInstrumentType(bool isBass);
    
    void setNoiseGateEnabled(bool enabled);
    void setNoiseGateThreshold(float threshold);
    
    void setCompressorEnabled(bool enabled);
    void setCompressorRatio(float ratio);
    
    void setReverbEnabled(bool enabled);
    void setReverbSize(float size);
    void setReverbDamping(float damping);
    
    void setDelayEnabled(bool enabled);
    void setDelayTime(float timeMs);
    void setDelayFeedback(float feedback);

private:
    juce::dsp::NoiseGate<float> noiseGate;
    juce::dsp::Compressor<float> compressor;
    juce::dsp::Reverb reverb;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
    juce::dsp::Gain<float> delayMix;
    
    juce::AudioBuffer<float> delayBuffer;
    
    bool isBassMode = false;
    bool noiseGateEnabled = false;
    bool compressorEnabled = false;
    bool reverbEnabled = false;
    bool delayEnabled = false;
    bool chorusEnabled = false;
    bool phaserEnabled = false;
    bool vintageFilterEnabled = false;
    
    float delayTimeMs = 250.0f;
    float delayFeedback = 0.3f;
    double sampleRate = 44100.0;
    
    void updateNoiseGate();
    void updateCompressor();
    void updateReverb();
    void updateDelay();
};
