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
    
    void setChorusEnabled(bool enabled);
    void setChorusRate(float rate);
    void setChorusDepth(float depth);
    
    void setPhaserEnabled(bool enabled);
    void setPhaserRate(float rate);
    void setPhaserDepth(float depth);
    
    void setVintageFilterEnabled(bool enabled);
    void setVintageFilterCutoff(float cutoff);
    void setVintageFilterResonance(float resonance);

private:
    juce::dsp::NoiseGate<float> noiseGate;
    juce::dsp::Compressor<float> compressor;
    juce::dsp::Reverb reverb;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
    juce::dsp::Gain<float> delayMix;
    juce::dsp::Chorus<float> chorus;
    juce::dsp::Phaser<float> phaser;
    juce::dsp::LadderFilter<float> vintageFilter;
    
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
    
    float noiseGateThreshold = -40.0f;
    float compressorRatio = 4.0f;
    float reverbSize = 0.5f;
    float reverbDamping = 0.5f;
    float chorusRate = 0.5f;
    float chorusDepth = 0.3f;
    float phaserRate = 0.3f;
    float phaserDepth = 0.4f;
    float filterCutoff = 2000.0f;
    float filterResonance = 0.3f;
    
    void updateNoiseGate();
    void updateCompressor();
    void updateReverb();
    void updateDelay();
};
