#include "CabSimulation.h"

CabSimulation::CabSimulation()
{
}

CabSimulation::~CabSimulation()
{
}

void CabSimulation::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    
    convolution.prepare(spec);
    lowPassFilter.prepare(spec);
    highPassFilter.prepare(spec);
    micColorFilter.prepare(spec);
    
    generateSyntheticIR();
    updateFilters();
}

void CabSimulation::process(juce::dsp::ProcessContextReplacing<float>& context)
{
    convolution.process(context);
    lowPassFilter.process(context);
    highPassFilter.process(context);
    micColorFilter.process(context);
}

void CabSimulation::reset()
{
    convolution.reset();
    lowPassFilter.reset();
    highPassFilter.reset();
    micColorFilter.reset();
}

void CabSimulation::setInstrumentType(bool isBass)
{
    isBassMode = isBass;
    updateFilters();
    generateSyntheticIR();
}

void CabSimulation::setCabModel(CabModel model)
{
    currentCabModel = model;
    generateSyntheticIR();
    updateFilters();
}

void CabSimulation::setMicType(MicType mic)
{
    currentMicType = mic;
    updateFilters();
}

void CabSimulation::setMicDistance(float distance)
{
    updateFilters();
}

void CabSimulation::updateFilters()
{
    float lowPassFreq, highPassFreq, micPeakFreq;
    
    if (isBassMode)
    {
        lowPassFreq = 5000.0f;
        highPassFreq = 40.0f;
        micPeakFreq = 2000.0f;
    }
    else
    {
        lowPassFreq = 8000.0f;
        highPassFreq = 80.0f;
        micPeakFreq = 5000.0f;
    }
    
    switch (currentMicType)
    {
        case Dynamic57:
            micPeakFreq *= 1.0f;
            break;
        case Dynamic421:
            micPeakFreq *= 0.8f;
            break;
        case Condenser414:
            micPeakFreq *= 1.2f;
            lowPassFreq *= 1.5f;
            break;
        case Ribbon121:
            micPeakFreq *= 0.6f;
            lowPassFreq *= 0.7f;
            break;
    }
    
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, lowPassFreq);
    *highPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, highPassFreq);
    *micColorFilter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, micPeakFreq, 1.0f, 1.2f);
}

void CabSimulation::generateSyntheticIR()
{
    const int irLength = 2048;
    juce::AudioBuffer<float> impulseResponse(2, irLength);
    impulseResponse.clear();
    
    auto* leftChannel = impulseResponse.getWritePointer(0);
    auto* rightChannel = impulseResponse.getWritePointer(1);
    
    juce::Random random;
    
    for (int i = 0; i < irLength; ++i)
    {
        float t = (float)i / (float)irLength;
        float envelope = std::exp(-t * 8.0f);
        
        float resonantFreq = isBassMode ? 100.0f : 200.0f;
        float sample = envelope * std::sin(2.0f * juce::MathConstants<float>::pi * resonantFreq * t / sampleRate);
        
        sample += envelope * 0.3f * std::sin(2.0f * juce::MathConstants<float>::pi * resonantFreq * 2.0f * t / sampleRate);
        sample += envelope * 0.1f * std::sin(2.0f * juce::MathConstants<float>::pi * resonantFreq * 3.0f * t / sampleRate);
        
        sample += envelope * 0.05f * (random.nextFloat() * 2.0f - 1.0f);
        
        leftChannel[i] = sample;
        rightChannel[i] = sample * 0.9f;
    }
    
    convolution.loadImpulseResponse(std::move(impulseResponse), sampleRate, 
                                   juce::dsp::Convolution::Stereo::yes, 
                                   juce::dsp::Convolution::Trim::yes, 
                                   juce::dsp::Convolution::Normalise::yes);
}
