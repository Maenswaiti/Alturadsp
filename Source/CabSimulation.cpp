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
    int irLength = static_cast<int>(sampleRate * 0.25); // Longer IR for better quality
    closeMicIR.resize(irLength);
    farMicIR.resize(irLength);
    roomIR.resize(static_cast<int>(sampleRate * 0.5));
    
    double baseFreq = isBassMode ? 45.0 : 82.0;
    std::vector<double> resonantFreqs = {baseFreq, baseFreq * 2.3, baseFreq * 4.7, baseFreq * 7.1};
    double cabinetResonance = getCabinetResonanceFreq(currentCabModel);
    
    for (int i = 0; i < irLength; ++i)
    {
        double t = static_cast<double>(i) / sampleRate;
        double envelope = std::exp(-t * 12.0) * (1.0 + 0.2 * std::sin(t * cabinetResonance));
        
        double signal = 0.0;
        for (size_t h = 0; h < resonantFreqs.size(); ++h)
        {
            double harmonic = std::sin(2.0 * juce::MathConstants<double>::pi * resonantFreqs[h] * t);
            signal += harmonic * envelope * (1.0 / (h + 1));
        }
        
        signal *= getCabinetColorationFactor(currentCabModel, t);
        
        closeMicIR[i] = static_cast<float>(signal * 0.9);
        farMicIR[i] = static_cast<float>(signal * 0.7 * (1.0 + 0.1 * std::sin(t * 30.0)));
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
