#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class AmpSection : public juce::Component
{
public:
    AmpSection(AlturadspAudioProcessor& processor);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    AlturadspAudioProcessor& audioProcessor;
    juce::ComboBox ampModelSelector;
    juce::Slider gainSlider, bassSlider, midSlider, trebleSlider, presenceSlider;
    juce::Label ampModelLabel, gainLabel, bassLabel, midLabel, trebleLabel, presenceLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ampModelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment, bassAttachment, midAttachment, trebleAttachment, presenceAttachment;
    
    void setupControls();
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text);
};
