#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class EffectsSection : public juce::Component
{
public:
    EffectsSection(AlturadspAudioProcessor& processor);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    AlturadspAudioProcessor& audioProcessor;
    
    juce::ToggleButton noiseGateToggle, compressorToggle, reverbToggle, delayToggle;
    juce::Slider noiseGateThresholdSlider, compressorRatioSlider;
    juce::Slider reverbSizeSlider, reverbDampingSlider;
    juce::Slider delayTimeSlider, delayFeedbackSlider;
    
    juce::Label noiseGateLabel, compressorLabel, reverbLabel, delayLabel;
    juce::Label thresholdLabel, ratioLabel, sizeLabel, dampingLabel, timeLabel, feedbackLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> noiseGateAttachment, compressorAttachment, reverbAttachment, delayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment, ratioAttachment, sizeAttachment, dampingAttachment, timeAttachment, feedbackAttachment;
    
    void setupControls();
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text);
};
