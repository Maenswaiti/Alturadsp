#pragma once

#include <JuceHeader.h>

class AmpModeling
{
public:
    AmpModeling();
    ~AmpModeling();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::ProcessContextReplacing<float>& context);
    void reset();
    
    void setInstrumentType(bool isBass);
    
    enum AmpModel
    {
        Clean = 0,
        Crunch,
        Lead,
        HighGain,
        Vintage,
        Modern
    };
    
    void setAmpModel(AmpModel model);
    void setGain(float gain);
    void setBass(float bass);
    void setMid(float mid);
    void setTreble(float treble);
    void setPresence(float presence);

private:
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowShelf;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> midPeak;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highShelf;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> presenceFilter;
    
    juce::dsp::WaveShaper<float> tubeDistortion;
    juce::dsp::Gain<float> inputGain;
    juce::dsp::Gain<float> outputGain;
    
    AmpModel currentModel = Clean;
    bool isBassMode = false;
    double sampleRate = 44100.0;
    
    void updateFilters();
    void updateDistortion();
    
    static float tubeWaveShaper(float input);
    static float transistorWaveShaper(float input);
    static float modernWaveShaper(float input);
};
