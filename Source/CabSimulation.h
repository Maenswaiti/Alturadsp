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
    void setMicBlend(float blend);
    void setRoomSize(float size);

private:
    juce::dsp::Convolution closeMicConvolution;
    juce::dsp::Convolution farMicConvolution;
    juce::dsp::Convolution roomConvolution;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowPassFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highPassFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> resonanceFilter;
    juce::dsp::Reverb roomReverb;
    
    juce::SmoothedValue<float> micBlendSmoothed;
    juce::SmoothedValue<float> roomSizeSmoothed;
    
    CabModel currentCabModel = Vintage4x12;
    MicType currentMicType = Dynamic57;
    bool isBassMode = false;
    float micDistance = 0.5f;
    float micBlend = 0.0f;
    float roomSize = 0.3f;
    double sampleRate = 44100.0;
    
    std::vector<float> closeMicIR;
    std::vector<float> farMicIR;
    std::vector<float> roomIR;
    
    void updateFilters();
    void loadImpulseResponse();
    void generateHighQualityIR();
    void generateNeuralIR();
    void applyMicCharacteristics(std::vector<float>& ir, MicType micType);
    void applyAdvancedMicCharacteristics(std::vector<float>& ir, MicType micType);
    void applyCabinetResonance(std::vector<float>& ir, CabModel cabModel);
    void updateMicBlending();
    double getCabinetResonanceFreq(CabModel model);
    double getCabinetColorationFactor(CabModel model, double time);
};
