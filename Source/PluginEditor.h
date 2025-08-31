#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class AlturadspAudioProcessorEditor : public juce::AudioProcessorEditor,
                                      public juce::Timer
{
public:
    AlturadspAudioProcessorEditor (AlturadspAudioProcessor&);
    ~AlturadspAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    AlturadspAudioProcessor& audioProcessor;

    juce::TabbedComponent mainTabs;
    
    std::unique_ptr<juce::Component> ampSection;
    std::unique_ptr<juce::Component> cabSection;
    std::unique_ptr<juce::Component> effectsSection;
    std::unique_ptr<juce::Component> presetSection;
    
    juce::ComboBox instrumentSelector;
    juce::Label instrumentLabel;
    
    juce::Colour backgroundColour;
    
    void setupUI();
    void updateInstrumentType();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AlturadspAudioProcessorEditor)
};
