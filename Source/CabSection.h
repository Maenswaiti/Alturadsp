#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class CabSection : public juce::Component
{
public:
    CabSection(AlturadspAudioProcessor& processor);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    AlturadspAudioProcessor& audioProcessor;
    juce::ComboBox cabModelSelector, micTypeSelector;
    juce::Slider micDistanceSlider;
    juce::Label cabModelLabel, micTypeLabel, micDistanceLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> cabModelAttachment, micTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> micDistanceAttachment;
    
    void setupControls();
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text);
};
