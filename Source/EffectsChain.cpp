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
    
    delayLine.setMaximumDelayInSamples(static_cast<int>(sampleRate * 2.0));
    
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
                float output = input + delayedSample * 0.5f;
                
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
    updateNoiseGate();
}

void EffectsChain::setCompressorEnabled(bool enabled)
{
    compressorEnabled = enabled;
}

void EffectsChain::setCompressorRatio(float ratio)
{
    updateCompressor();
}

void EffectsChain::setReverbEnabled(bool enabled)
{
    reverbEnabled = enabled;
}

void EffectsChain::setReverbSize(float size)
{
    updateReverb();
}

void EffectsChain::setReverbDamping(float damping)
{
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

void EffectsChain::updateNoiseGate()
{
    noiseGate.setThreshold(isBassMode ? -50.0f : -40.0f);
    noiseGate.setRatio(10.0f);
    noiseGate.setAttack(1.0f);
    noiseGate.setRelease(100.0f);
}

void EffectsChain::updateCompressor()
{
    compressor.setThreshold(-12.0f);
    compressor.setRatio(4.0f);
    compressor.setAttack(5.0f);
    compressor.setRelease(100.0f);
}

void EffectsChain::updateReverb()
{
    juce::Reverb::Parameters reverbParams;
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
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
