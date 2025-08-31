#pragma once

#include <JuceHeader.h>

class CabSimulation
{
public:
    CabSimulation();
    ~CabSimulation();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::ProcessContextReplacing<float>& context);
    void reset();
    
    void setInstrumentType(bool isBass);
    
    enum CabModel
    {
        Vintage4x12 = 0,
        Modern4x12,
        Combo2x12,
        Studio1x12,
        Bass8x10,
        Bass4x10
    };
    
    enum MicType
    {
        Dynamic57 = 0,
        Dynamic421,
        Condenser414,
        Ribbon121
    };
    
    void setCabModel(CabModel model);
    void setMicType(MicType mic);
    void setMicDistance(float distance);

private:
    juce::dsp::Convolution convolution;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowPassFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highPassFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> micColorFilter;
    
    CabModel currentCabModel = Vintage4x12;
    MicType currentMicType = Dynamic57;
    bool isBassMode = false;
    double sampleRate = 44100.0;
    
    void updateFilters();
    void loadImpulseResponse();
    void generateSyntheticIR();
};
