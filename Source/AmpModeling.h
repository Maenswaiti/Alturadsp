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
    
    juce::dsp::Gain<float> inputGain;
    juce::dsp::Gain<float> outputGain;
    juce::dsp::Oversampling<float> oversampling;
    juce::dsp::LadderFilter<float> ladderFilter;
    
    juce::SmoothedValue<float> gainSmoothed;
    juce::SmoothedValue<float> bassSmoothed;
    juce::SmoothedValue<float> midSmoothed;
    juce::SmoothedValue<float> trebleSmoothed;
    juce::SmoothedValue<float> presenceSmoothed;
    
    AmpModel currentModel = Clean;
    bool isBassMode = false;
    double sampleRate = 44100.0;
    
    float previousSample = 0.0f;
    float tubeTemperature = 1.0f;
    
    void updateFilters();
    void updateDistortion();
    
    static float advancedTubeWaveShaper(float input, float drive);
    static float asymmetricTubeWaveShaper(float input, float asymmetry);
    static float volterraSeriesWaveShaper(float input, float harmonics);
    static float dynamicTubeCompression(float input, float compression);
    static float tubeWaveShaper(float input);
    static float transistorWaveShaper(float input);
    static float modernWaveShaper(float input);
};
