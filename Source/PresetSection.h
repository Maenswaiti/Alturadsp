#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class PresetSection : public juce::Component
{
public:
    PresetSection(AlturadspAudioProcessor& processor);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    AlturadspAudioProcessor& audioProcessor;
    
    juce::ComboBox presetSelector;
    juce::TextButton saveButton, loadButton, deleteButton;
    juce::TextEditor presetNameEditor;
    juce::Label presetLabel, nameLabel;
    
    void setupControls();
    void updatePresetList();
    void saveCurrentPreset();
    void loadSelectedPreset();
    void deleteSelectedPreset();
};
