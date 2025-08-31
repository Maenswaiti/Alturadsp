#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class EffectModule : public juce::Component
{
public:
    EffectModule(const juce::String& name, const juce::String& description);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void setEnabled(bool enabled);
    bool isEnabled() const { return effectEnabled; }
    
    void addControl(juce::Component* control, const juce::String& label = "");
    
    std::function<void(bool)> onEnabledChanged;
    
    juce::ToggleButton enableButton;
    
private:
    juce::String effectName;
    juce::String effectDescription;
    bool effectEnabled = false;
    
    juce::OwnedArray<juce::Component> controls;
    juce::StringArray controlLabels;
    
    void drawEffectIcon(juce::Graphics& g, juce::Rectangle<float> bounds);
};

class EffectsSection : public juce::Component
{
public:
    EffectsSection(AlturadspAudioProcessor& processor);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    AlturadspAudioProcessor& audioProcessor;
    
    std::unique_ptr<EffectModule> noiseGateModule;
    std::unique_ptr<EffectModule> compressorModule;
    std::unique_ptr<EffectModule> chorusModule;
    std::unique_ptr<EffectModule> phaserModule;
    std::unique_ptr<EffectModule> delayModule;
    std::unique_ptr<EffectModule> reverbModule;
    std::unique_ptr<EffectModule> vintageFilterModule;
    
    juce::Slider noiseGateThresholdSlider, compressorRatioSlider;
    juce::Slider chorusRateSlider, chorusDepthSlider;
    juce::Slider phaserRateSlider, phaserDepthSlider;
    juce::Slider delayTimeSlider, delayFeedbackSlider;
    juce::Slider reverbSizeSlider, reverbDampingSlider;
    juce::Slider filterCutoffSlider, filterResonanceSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> noiseGateAttachment, compressorAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> chorusAttachment, phaserAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> delayAttachment, reverbAttachment, filterAttachment;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment, ratioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusRateAttachment, chorusDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phaserRateAttachment, phaserDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment, delayFeedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbSizeAttachment, reverbDampingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment, filterResonanceAttachment;
    
    void setupEffectModules();
    void setupSlider(juce::Slider& slider, const juce::String& paramId, float min, float max, float defaultVal);
};
