#include "CabSimulation.h"

CabSimulation::CabSimulation()
{
    micBlendSmoothed.setCurrentAndTargetValue(0.0f);
    roomSizeSmoothed.setCurrentAndTargetValue(0.3f);
}

CabSimulation::~CabSimulation()
{
}

void CabSimulation::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    
    closeMicConvolution.prepare(spec);
    farMicConvolution.prepare(spec);
    roomConvolution.prepare(spec);
    lowPassFilter.prepare(spec);
    highPassFilter.prepare(spec);
    resonanceFilter.prepare(spec);
    roomReverb.prepare(spec);
    
    micBlendSmoothed.reset(spec.sampleRate, 0.05);
    roomSizeSmoothed.reset(spec.sampleRate, 0.1);
    
    generateHighQualityIR();
    updateFilters();
}

void CabSimulation::process(juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();
    
    juce::dsp::AudioBlock<float> closeMicBlock(outputBlock);
    juce::dsp::AudioBlock<float> farMicBlock(outputBlock);
    juce::dsp::AudioBlock<float> roomBlock(outputBlock);
    
    juce::dsp::ProcessContextReplacing<float> closeMicContext(closeMicBlock);
    juce::dsp::ProcessContextReplacing<float> farMicContext(farMicBlock);
    juce::dsp::ProcessContextReplacing<float> roomContext(roomBlock);
    
    closeMicContext.getOutputBlock().copyFrom(inputBlock);
    farMicContext.getOutputBlock().copyFrom(inputBlock);
    roomContext.getOutputBlock().copyFrom(inputBlock);
    
    resonanceFilter.process(closeMicContext);
    closeMicConvolution.process(closeMicContext);
    
    lowPassFilter.process(farMicContext);
    farMicConvolution.process(farMicContext);
    
    highPassFilter.process(roomContext);
    roomConvolution.process(roomContext);
    roomReverb.process(roomContext);
    
    updateMicBlending();
    
    for (size_t channel = 0; channel < outputBlock.getNumChannels(); ++channel)
    {
        auto* outputData = outputBlock.getChannelPointer(channel);
        auto* closeMicData = closeMicBlock.getChannelPointer(channel);
        auto* farMicData = farMicBlock.getChannelPointer(channel);
        auto* roomData = roomBlock.getChannelPointer(channel);
        
        for (size_t sample = 0; sample < outputBlock.getNumSamples(); ++sample)
        {
            float blend = micBlendSmoothed.getNextValue();
            float roomAmount = roomSizeSmoothed.getNextValue();
            
            float closeMicLevel = (1.0f - blend) * (1.0f - roomAmount * 0.5f);
            float farMicLevel = blend * (1.0f - roomAmount * 0.3f);
            float roomLevel = roomAmount * 0.4f;
            
            outputData[sample] = closeMicData[sample] * closeMicLevel +
                               farMicData[sample] * farMicLevel +
                               roomData[sample] * roomLevel;
        }
    }
}

void CabSimulation::reset()
{
    closeMicConvolution.reset();
    farMicConvolution.reset();
    roomConvolution.reset();
    lowPassFilter.reset();
    highPassFilter.reset();
    resonanceFilter.reset();
    roomReverb.reset();
}

void CabSimulation::setInstrumentType(bool isBass)
{
    isBassMode = isBass;
    updateFilters();
    generateHighQualityIR();
}

void CabSimulation::setCabModel(CabModel model)
{
    currentCabModel = model;
    generateHighQualityIR();
    updateFilters();
}

void CabSimulation::setMicType(MicType mic)
{
    currentMicType = mic;
    updateFilters();
}

void CabSimulation::setMicDistance(float distance)
{
    micDistance = juce::jlimit(0.0f, 1.0f, distance);
    updateFilters();
}

void CabSimulation::setMicBlend(float blend)
{
    micBlend = juce::jlimit(0.0f, 1.0f, blend);
    micBlendSmoothed.setTargetValue(micBlend);
}

void CabSimulation::setRoomSize(float size)
{
    roomSize = juce::jlimit(0.0f, 1.0f, size);
    roomSizeSmoothed.setTargetValue(roomSize);
    
    juce::Reverb::Parameters reverbParams;
    reverbParams.roomSize = roomSize;
    reverbParams.damping = 0.5f + roomSize * 0.3f;
    reverbParams.wetLevel = roomSize * 0.3f;
    reverbParams.dryLevel = 1.0f - roomSize * 0.2f;
    roomReverb.setParameters(reverbParams);
}

void CabSimulation::updateFilters()
{
    double lowPassFreq = isBassMode ? 4000.0 : 7000.0;
    double highPassFreq = isBassMode ? 30.0 : 60.0;
    double resonanceFreq = isBassMode ? 180.0 : 350.0;
    
    lowPassFreq *= (1.0 + micDistance * 0.3);
    highPassFreq *= (1.0 - micDistance * 0.2);
    
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, lowPassFreq, 0.8);
    *highPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, highPassFreq, 0.7);
    *resonanceFilter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, resonanceFreq, 1.5, juce::Decibels::decibelsToGain(3.0));
}

void CabSimulation::updateMicBlending()
{
}

void CabSimulation::generateHighQualityIR()
{
    generateNeuralIR();
}

void CabSimulation::generateNeuralIR()
{
    int irLength = static_cast<int>(sampleRate * 0.6); // Longer for better quality
    closeMicIR.resize(irLength);
    farMicIR.resize(irLength);
    roomIR.resize(static_cast<int>(sampleRate * 1.2)); // Extended room response
    
    double baseFreq = isBassMode ? 35.0 : 75.0; // More accurate fundamental frequencies
    
    std::vector<double> resonantFreqs;
    std::vector<double> resonantAmplitudes;
    std::vector<double> resonantDecays;
    
    switch (currentCabModel)
    {
        case Vintage4x12:
            resonantFreqs = {baseFreq, baseFreq * 2.1, baseFreq * 3.8, baseFreq * 6.2, baseFreq * 9.5, baseFreq * 13.2};
            resonantAmplitudes = {1.0, 0.7, 0.5, 0.3, 0.2, 0.1};
            resonantDecays = {6.0, 8.0, 10.0, 12.0, 15.0, 18.0};
            break;
        case Modern4x12:
            resonantFreqs = {baseFreq, baseFreq * 2.3, baseFreq * 4.1, baseFreq * 6.8, baseFreq * 10.2, baseFreq * 14.5};
            resonantAmplitudes = {1.0, 0.8, 0.6, 0.4, 0.25, 0.12};
            resonantDecays = {7.0, 9.0, 11.0, 13.0, 16.0, 20.0};
            break;
        case Combo2x12:
            resonantFreqs = {baseFreq * 1.2, baseFreq * 2.5, baseFreq * 4.3, baseFreq * 7.1, baseFreq * 11.0};
            resonantAmplitudes = {1.0, 0.6, 0.4, 0.25, 0.15};
            resonantDecays = {5.0, 7.0, 9.0, 11.0, 14.0};
            break;
        case Studio1x12:
            resonantFreqs = {baseFreq * 1.1, baseFreq * 2.4, baseFreq * 4.2, baseFreq * 6.9, baseFreq * 10.5};
            resonantAmplitudes = {1.0, 0.65, 0.45, 0.28, 0.18};
            resonantDecays = {5.5, 7.5, 9.5, 11.5, 14.5};
            break;
        case Bass8x10:
            resonantFreqs = {baseFreq, baseFreq * 1.8, baseFreq * 3.2, baseFreq * 5.1, baseFreq * 7.8};
            resonantAmplitudes = {1.0, 0.9, 0.7, 0.5, 0.3};
            resonantDecays = {8.0, 10.0, 12.0, 14.0, 17.0};
            break;
        case Bass4x10:
            resonantFreqs = {baseFreq, baseFreq * 1.9, baseFreq * 3.4, baseFreq * 5.3, baseFreq * 8.1};
            resonantAmplitudes = {1.0, 0.85, 0.65, 0.45, 0.25};
            resonantDecays = {7.5, 9.5, 11.5, 13.5, 16.5};
            break;
        default:
            resonantFreqs = {baseFreq, baseFreq * 2.2, baseFreq * 4.0, baseFreq * 6.5, baseFreq * 9.8};
            resonantAmplitudes = {1.0, 0.7, 0.5, 0.3, 0.2};
            resonantDecays = {6.0, 8.0, 10.0, 12.0, 15.0};
            break;
    }
    
    double cabinetResonance = getCabinetResonanceFreq(currentCabModel);
    
    for (int i = 0; i < irLength; ++i)
    {
        double t = static_cast<double>(i) / sampleRate;
        double signal = 0.0;
        
        double primaryEnvelope = std::exp(-t * 5.0) * (1.0 + 0.4 * std::sin(t * cabinetResonance * 2.0 * juce::MathConstants<double>::pi));
        
        for (size_t h = 0; h < resonantFreqs.size(); ++h)
        {
            double harmonic = std::sin(2.0 * juce::MathConstants<double>::pi * resonantFreqs[h] * t);
            double harmonicEnvelope = std::exp(-t * resonantDecays[h]);
            signal += harmonic * harmonicEnvelope * resonantAmplitudes[h] * primaryEnvelope;
        }
        
        signal *= getCabinetColorationFactor(currentCabModel, t);
        
        double woodResonance = 0.15 * std::sin(2.0 * juce::MathConstants<double>::pi * (baseFreq * 0.7) * t) * std::exp(-t * 4.0);
        double airMovement = 0.08 * std::sin(2.0 * juce::MathConstants<double>::pi * (baseFreq * 1.3) * t) * std::exp(-t * 8.0);
        double speakerCone = 0.12 * std::sin(2.0 * juce::MathConstants<double>::pi * (baseFreq * 2.8) * t) * std::exp(-t * 12.0);
        
        double portResonance = 0.0;
        if (currentCabModel == Modern4x12 || currentCabModel == Combo2x12)
        {
            portResonance = 0.1 * std::sin(2.0 * juce::MathConstants<double>::pi * (baseFreq * 0.9) * t) * std::exp(-t * 6.0);
        }
        
        signal += woodResonance + airMovement + speakerCone + portResonance;
        
        closeMicIR[i] = static_cast<float>(signal * 0.9);
        
        double roomInteraction = 1.0 + 0.2 * std::sin(t * 18.0 * 2.0 * juce::MathConstants<double>::pi);
        double phaseShift = 0.95; // Slight phase difference from distance
        farMicIR[i] = static_cast<float>(signal * 0.7 * roomInteraction * phaseShift);
    }
    
    for (int i = 0; i < static_cast<int>(roomIR.size()); ++i)
    {
        double t = static_cast<double>(i) / sampleRate;
        double roomEnvelope = std::exp(-t * 4.0);
        double earlyReflections = 0.0;
        
        double midFreq = baseFreq * 2.5;
        double highFreq = baseFreq * 4.2;
        
        if (t > 0.01)
        {
            earlyReflections += 0.3 * std::sin(2.0 * juce::MathConstants<double>::pi * baseFreq * 0.9 * t) * roomEnvelope;
            earlyReflections += 0.2 * std::sin(2.0 * juce::MathConstants<double>::pi * midFreq * 0.8 * t) * roomEnvelope;
        }
        
        if (t > 0.02)
        {
            earlyReflections += 0.15 * std::sin(2.0 * juce::MathConstants<double>::pi * highFreq * 0.6 * t) * roomEnvelope;
        }
        
        roomIR[i] = static_cast<float>(earlyReflections);
    }
    
    if (!farMicIR.empty())
    {
        farMicConvolution.loadImpulseResponse(farMicIR.data(), farMicIR.size(), 
                                            juce::dsp::Convolution::Stereo::no, 
                                            juce::dsp::Convolution::Trim::yes, 
                                            farMicIR.size(),
                                            juce::dsp::Convolution::Normalise::yes);
    }
    
    if (!roomIR.empty())
    {
        roomConvolution.loadImpulseResponse(roomIR.data(), roomIR.size(), 
                                          juce::dsp::Convolution::Stereo::no, 
                                          juce::dsp::Convolution::Trim::yes, 
                                          roomIR.size(),
                                          juce::dsp::Convolution::Normalise::yes);
    }
    
    applyAdvancedMicCharacteristics(closeMicIR, currentMicType);
    applyCabinetResonance(closeMicIR, currentCabModel);
    
    if (!closeMicIR.empty())
    {
        closeMicConvolution.loadImpulseResponse(closeMicIR.data(), closeMicIR.size(), 
                                              juce::dsp::Convolution::Stereo::no, 
                                              juce::dsp::Convolution::Trim::yes, 
                                              closeMicIR.size(),
                                              juce::dsp::Convolution::Normalise::yes);
    }
}

void CabSimulation::applyMicCharacteristics(std::vector<float>& ir, MicType micType)
{
    if (ir.empty()) return;
    
    for (size_t i = 0; i < ir.size(); ++i)
    {
        double t = static_cast<double>(i) / sampleRate;
        double micResponse = 1.0;
        
        switch (micType)
        {
            case Dynamic57:
                micResponse = 1.0 + 0.3 * std::sin(t * 5000.0 * 2.0 * juce::MathConstants<double>::pi);
                micResponse *= std::exp(-t * 15000.0 * 0.1);
                break;
            case Dynamic421:
                micResponse = 1.0 + 0.2 * std::sin(t * 200.0 * 2.0 * juce::MathConstants<double>::pi);
                micResponse += 0.1 * std::sin(t * 3000.0 * 2.0 * juce::MathConstants<double>::pi);
                break;
            case Condenser414:
                micResponse = 1.0 + 0.15 * std::sin(t * 10000.0 * 2.0 * juce::MathConstants<double>::pi);
                micResponse += 0.1 * std::sin(t * 15000.0 * 2.0 * juce::MathConstants<double>::pi);
                break;
            case Ribbon121:
                micResponse = 1.0 - 0.2 * std::sin(t * 8000.0 * 2.0 * juce::MathConstants<double>::pi);
                micResponse *= std::exp(-t * 12000.0 * 0.05);
                break;
        }
        
        ir[i] *= static_cast<float>(micResponse);
    }
}

void CabSimulation::applyCabinetResonance(std::vector<float>& ir, CabModel cabModel)
{
    if (ir.empty()) return;
    
    for (size_t i = 0; i < ir.size(); ++i)
    {
        double t = static_cast<double>(i) / sampleRate;
        double cabinetResponse = 1.0;
        
        switch (cabModel)
        {
            case Vintage4x12:
                cabinetResponse = 1.0 + 0.4 * std::sin(t * 100.0 * 2.0 * juce::MathConstants<double>::pi);
                cabinetResponse += 0.3 * std::sin(t * 2500.0 * 2.0 * juce::MathConstants<double>::pi);
                cabinetResponse *= std::exp(-t * 8000.0 * 0.1);
                break;
            case Modern4x12:
                cabinetResponse = 1.0 + 0.2 * std::sin(t * 80.0 * 2.0 * juce::MathConstants<double>::pi);
                cabinetResponse += 0.4 * std::sin(t * 3000.0 * 2.0 * juce::MathConstants<double>::pi);
                break;
            case Combo2x12:
                cabinetResponse = 1.0 + 0.3 * std::sin(t * 120.0 * 2.0 * juce::MathConstants<double>::pi);
                cabinetResponse += 0.2 * std::sin(t * 2000.0 * 2.0 * juce::MathConstants<double>::pi);
                break;
            case Studio1x12:
                cabinetResponse = 1.0 + 0.25 * std::sin(t * 150.0 * 2.0 * juce::MathConstants<double>::pi);
                cabinetResponse += 0.3 * std::sin(t * 2800.0 * 2.0 * juce::MathConstants<double>::pi);
                break;
            case Bass8x10:
                cabinetResponse = 1.0 + 0.6 * std::sin(t * 60.0 * 2.0 * juce::MathConstants<double>::pi);
                cabinetResponse += 0.3 * std::sin(t * 800.0 * 2.0 * juce::MathConstants<double>::pi);
                break;
            case Bass4x10:
                cabinetResponse = 1.0 + 0.4 * std::sin(t * 80.0 * 2.0 * juce::MathConstants<double>::pi);
                cabinetResponse += 0.4 * std::sin(t * 1200.0 * 2.0 * juce::MathConstants<double>::pi);
                break;
        }
        
        ir[i] *= static_cast<float>(cabinetResponse);
    }
}

double CabSimulation::getCabinetResonanceFreq(CabModel model)
{
    switch (model)
    {
        case Vintage4x12: return 95.0;
        case Modern4x12: return 85.0;
        case Combo2x12: return 110.0;
        case Studio1x12: return 130.0;
        case Bass8x10: return 55.0;
        case Bass4x10: return 65.0;
        default: return 100.0;
    }
}

double CabSimulation::getCabinetColorationFactor(CabModel model, double time)
{
    switch (model)
    {
        case Vintage4x12: 
            return 1.0 + 0.3 * std::exp(-time * 8.0) * std::sin(time * 2500.0 * 2.0 * juce::MathConstants<double>::pi);
        case Modern4x12:
            return 1.0 + 0.2 * std::exp(-time * 10.0) * std::sin(time * 3200.0 * 2.0 * juce::MathConstants<double>::pi);
        case Combo2x12:
            return 1.0 + 0.25 * std::exp(-time * 9.0) * std::sin(time * 2800.0 * 2.0 * juce::MathConstants<double>::pi);
        case Studio1x12:
            return 1.0 + 0.2 * std::exp(-time * 11.0) * std::sin(time * 3000.0 * 2.0 * juce::MathConstants<double>::pi);
        case Bass8x10:
            return 1.0 + 0.4 * std::exp(-time * 6.0) * std::sin(time * 800.0 * 2.0 * juce::MathConstants<double>::pi);
        case Bass4x10:
            return 1.0 + 0.35 * std::exp(-time * 7.0) * std::sin(time * 1000.0 * 2.0 * juce::MathConstants<double>::pi);
        default:
            return 1.0;
    }
}

void CabSimulation::applyAdvancedMicCharacteristics(std::vector<float>& ir, MicType micType)
{
    if (ir.empty()) return;
    
    for (size_t i = 0; i < ir.size(); ++i)
    {
        double t = static_cast<double>(i) / sampleRate;
        double micResponse = 1.0;
        
        switch (micType)
        {
            case Dynamic57:
                micResponse = 1.0 + 0.4 * std::sin(t * 5000.0 * 2.0 * juce::MathConstants<double>::pi);
                micResponse *= std::exp(-t * 12000.0 * 0.08);
                micResponse += 0.1 * std::sin(t * 3000.0 * 2.0 * juce::MathConstants<double>::pi);
                break;
            case Dynamic421:
                micResponse = 1.0 + 0.3 * std::sin(t * 200.0 * 2.0 * juce::MathConstants<double>::pi);
                micResponse += 0.2 * std::sin(t * 3500.0 * 2.0 * juce::MathConstants<double>::pi);
                micResponse *= (1.0 + 0.1 * std::sin(t * 1000.0 * 2.0 * juce::MathConstants<double>::pi));
                break;
            case Condenser414:
                micResponse = 1.0 + 0.2 * std::sin(t * 12000.0 * 2.0 * juce::MathConstants<double>::pi);
                micResponse += 0.15 * std::sin(t * 18000.0 * 2.0 * juce::MathConstants<double>::pi);
                micResponse *= (1.0 + 0.05 * std::sin(t * 8000.0 * 2.0 * juce::MathConstants<double>::pi));
                break;
            case Ribbon121:
                micResponse = 1.0 - 0.3 * std::sin(t * 10000.0 * 2.0 * juce::MathConstants<double>::pi);
                micResponse *= std::exp(-t * 15000.0 * 0.04);
                micResponse += 0.1 * std::sin(t * 2000.0 * 2.0 * juce::MathConstants<double>::pi);
                break;
        }
        
        ir[i] *= static_cast<float>(micResponse);
    }
}
