#include "AmpModeling.h"

AmpModeling::AmpModeling()
{
    tubeDistortion.functionToUse = tubeWaveShaper;
}

AmpModeling::~AmpModeling()
{
}

void AmpModeling::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    
    lowShelf.prepare(spec);
    midPeak.prepare(spec);
    highShelf.prepare(spec);
    presenceFilter.prepare(spec);
    tubeDistortion.prepare(spec);
    inputGain.prepare(spec);
    outputGain.prepare(spec);
    
    updateFilters();
    updateDistortion();
}

void AmpModeling::process(juce::dsp::ProcessContextReplacing<float>& context)
{
    inputGain.process(context);
    lowShelf.process(context);
    midPeak.process(context);
    highShelf.process(context);
    tubeDistortion.process(context);
    presenceFilter.process(context);
    outputGain.process(context);
}

void AmpModeling::reset()
{
    lowShelf.reset();
    midPeak.reset();
    highShelf.reset();
    presenceFilter.reset();
    tubeDistortion.reset();
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
    inputGain.setGainDecibels(gain * 6.0f);
}

void AmpModeling::setBass(float bass)
{
    updateFilters();
}

void AmpModeling::setMid(float mid)
{
    updateFilters();
}

void AmpModeling::setTreble(float treble)
{
    updateFilters();
}

void AmpModeling::setPresence(float presence)
{
    updateFilters();
}

void AmpModeling::updateFilters()
{
    float bassFreq = isBassMode ? 80.0f : 100.0f;
    float midFreq = isBassMode ? 500.0f : 1000.0f;
    float trebleFreq = isBassMode ? 3000.0f : 5000.0f;
    float presenceFreq = isBassMode ? 5000.0f : 8000.0f;
    
    *lowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, bassFreq, 0.7f, 1.0f);
    *midPeak.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, midFreq, 2.0f, 1.0f);
    *highShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, trebleFreq, 0.7f, 1.0f);
    *presenceFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, presenceFreq, 0.7f, 1.0f);
}

void AmpModeling::updateDistortion()
{
    switch (currentModel)
    {
        case Clean:
            tubeDistortion.functionToUse = [](float x) { return x; };
            outputGain.setGainDecibels(0.0f);
            break;
        case Crunch:
            tubeDistortion.functionToUse = tubeWaveShaper;
            outputGain.setGainDecibels(-6.0f);
            break;
        case Lead:
            tubeDistortion.functionToUse = tubeWaveShaper;
            outputGain.setGainDecibels(-3.0f);
            break;
        case HighGain:
            tubeDistortion.functionToUse = modernWaveShaper;
            outputGain.setGainDecibels(-9.0f);
            break;
        case Vintage:
            tubeDistortion.functionToUse = tubeWaveShaper;
            outputGain.setGainDecibels(-6.0f);
            break;
        case Modern:
            tubeDistortion.functionToUse = modernWaveShaper;
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
