#include "EffectsChain.h"

EffectsChain::EffectsChain()
{
}

EffectsChain::~EffectsChain()
{
}

void EffectsChain::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    
    noiseGate.prepare(spec);
    compressor.prepare(spec);
    reverb.prepare(spec);
    delayLine.prepare(spec);
    delayMix.prepare(spec);
    chorus.prepare(spec);
    phaser.prepare(spec);
    vintageFilter.prepare(spec);
    
    delayLine.setMaximumDelayInSamples(static_cast<int>(sampleRate * 2.0));
    
    chorus.setRate(0.5f);
    chorus.setDepth(0.3f);
    chorus.setCentreDelay(7.0f);
    chorus.setFeedback(0.2f);
    chorus.setMix(0.5f);
    
    phaser.setRate(0.3f);
    phaser.setDepth(0.4f);
    phaser.setCentreFrequency(1000.0f);
    phaser.setFeedback(0.3f);
    phaser.setMix(0.5f);
    
    vintageFilter.setMode(juce::dsp::LadderFilterMode::LPF24);
    vintageFilter.setCutoffFrequencyHz(2000.0f);
    vintageFilter.setResonance(0.3f);
    vintageFilter.setDrive(1.2f);
    
    updateNoiseGate();
    updateCompressor();
    updateReverb();
    updateDelay();
}

void EffectsChain::processPreAmp(juce::dsp::ProcessContextReplacing<float>& context)
{
    if (noiseGateEnabled)
        noiseGate.process(context);
    
    if (compressorEnabled)
        compressor.process(context);
}

void EffectsChain::processPostAmp(juce::dsp::ProcessContextReplacing<float>& context)
{
    if (vintageFilterEnabled)
    {
        vintageFilter.process(context);
    }
    
    if (chorusEnabled)
    {
        chorus.process(context);
    }
    
    if (phaserEnabled)
    {
        phaser.process(context);
    }
    
    if (delayEnabled)
    {
        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        
        for (size_t channel = 0; channel < inputBlock.getNumChannels(); ++channel)
        {
            auto* inputSamples = inputBlock.getChannelPointer(channel);
            auto* outputSamples = outputBlock.getChannelPointer(channel);
            
            for (size_t sample = 0; sample < inputBlock.getNumSamples(); ++sample)
            {
                float delayedSample = delayLine.popSample(static_cast<int>(channel));
                float input = inputSamples[sample];
                float output = input + delayedSample * 0.4f;
                
                delayLine.pushSample(static_cast<int>(channel), input + delayedSample * delayFeedback);
                outputSamples[sample] = output;
            }
        }
    }
    
    if (reverbEnabled)
    {
        reverb.process(context);
    }
}

void EffectsChain::reset()
{
    noiseGate.reset();
    compressor.reset();
    reverb.reset();
    delayLine.reset();
    delayMix.reset();
    chorus.reset();
    phaser.reset();
    vintageFilter.reset();
}

void EffectsChain::setInstrumentType(bool isBass)
{
    isBassMode = isBass;
    updateNoiseGate();
    updateCompressor();
}

void EffectsChain::setNoiseGateEnabled(bool enabled)
{
    noiseGateEnabled = enabled;
}

void EffectsChain::setNoiseGateThreshold(float threshold)
{
    noiseGateThreshold = threshold;
    updateNoiseGate();
}

void EffectsChain::setCompressorEnabled(bool enabled)
{
    compressorEnabled = enabled;
}

void EffectsChain::setCompressorRatio(float ratio)
{
    compressorRatio = ratio;
    updateCompressor();
}

void EffectsChain::setReverbEnabled(bool enabled)
{
    reverbEnabled = enabled;
}

void EffectsChain::setReverbSize(float size)
{
    reverbSize = size;
    updateReverb();
}

void EffectsChain::setReverbDamping(float damping)
{
    reverbDamping = damping;
    updateReverb();
}

void EffectsChain::setDelayEnabled(bool enabled)
{
    delayEnabled = enabled;
}

void EffectsChain::setDelayTime(float timeMs)
{
    delayTimeMs = timeMs;
    updateDelay();
}

void EffectsChain::setDelayFeedback(float feedback)
{
    delayFeedback = feedback;
}

void EffectsChain::setChorusEnabled(bool enabled)
{
    chorusEnabled = enabled;
}

void EffectsChain::setPhaserEnabled(bool enabled)
{
    phaserEnabled = enabled;
}

void EffectsChain::setVintageFilterEnabled(bool enabled)
{
    vintageFilterEnabled = enabled;
}

void EffectsChain::setChorusRate(float rate)
{
    chorusRate = rate;
    chorus.setRate(rate);
}

void EffectsChain::setChorusDepth(float depth)
{
    chorusDepth = depth;
    chorus.setDepth(depth);
}

void EffectsChain::setPhaserRate(float rate)
{
    phaserRate = rate;
    phaser.setRate(rate);
}

void EffectsChain::setPhaserDepth(float depth)
{
    phaserDepth = depth;
    phaser.setDepth(depth);
}

void EffectsChain::setVintageFilterCutoff(float cutoff)
{
    filterCutoff = cutoff;
    vintageFilter.setCutoffFrequencyHz(cutoff);
}

void EffectsChain::setVintageFilterResonance(float resonance)
{
    filterResonance = resonance;
    vintageFilter.setResonance(resonance);
}

void EffectsChain::updateNoiseGate()
{
    noiseGate.setThreshold(noiseGateThreshold);
    noiseGate.setRatio(10.0f);
    noiseGate.setAttack(1.0f);
    noiseGate.setRelease(100.0f);
}

void EffectsChain::updateCompressor()
{
    compressor.setThreshold(-12.0f);
    compressor.setRatio(compressorRatio);
    compressor.setAttack(5.0f);
    compressor.setRelease(100.0f);
}

void EffectsChain::updateReverb()
{
    juce::Reverb::Parameters reverbParams;
    reverbParams.roomSize = reverbSize;
    reverbParams.damping = reverbDamping;
    reverbParams.wetLevel = 0.3f;
    reverbParams.dryLevel = 0.7f;
    reverbParams.width = 1.0f;
    reverbParams.freezeMode = 0.0f;
    
    reverb.setParameters(reverbParams);
}

void EffectsChain::updateDelay()
{
    float delaySamples = (delayTimeMs / 1000.0f) * static_cast<float>(sampleRate);
    delayLine.setDelay(delaySamples);
}
