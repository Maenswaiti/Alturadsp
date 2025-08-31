#include "PresetSection.h"

PresetSection::PresetSection(AlturadspAudioProcessor& processor) : audioProcessor(processor)
{
    setupControls();
    updatePresetList();
}

void PresetSection::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colour(0xff161b22), 0, 0,
                                 juce::Colour(0xff21262d), 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    auto headerArea = getLocalBounds().removeFromTop(60);
    g.setColour(juce::Colour(0xff0d1117).withAlpha(0.6f));
    g.fillRoundedRectangle(headerArea.reduced(10).toFloat(), 8.0f);
    
    g.setColour(juce::Colour(0xfff0f6fc));
    g.setFont(juce::FontOptions(20.0f, juce::Font::bold));
    g.drawText("PRESET MANAGER", headerArea, juce::Justification::centred);
}

void PresetSection::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(40);
    
    auto topRow = area.removeFromTop(40);
    presetSelector.setBounds(topRow.removeFromLeft(300).reduced(5));
    
    auto buttonRow = area.removeFromTop(40);
    loadButton.setBounds(buttonRow.removeFromLeft(80).reduced(5));
    saveButton.setBounds(buttonRow.removeFromLeft(80).reduced(5));
    deleteButton.setBounds(buttonRow.removeFromLeft(80).reduced(5));
    
    auto nameRow = area.removeFromTop(40);
    nameLabel.setBounds(nameRow.removeFromLeft(100));
    presetNameEditor.setBounds(nameRow.reduced(5));
}

void PresetSection::setupControls()
{
    addAndMakeVisible(presetSelector);
    presetSelector.onChange = [this] { loadSelectedPreset(); };
    
    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Load");
    loadButton.onClick = [this] { loadSelectedPreset(); };
    
    addAndMakeVisible(saveButton);
    saveButton.setButtonText("Save");
    saveButton.onClick = [this] { saveCurrentPreset(); };
    
    addAndMakeVisible(deleteButton);
    deleteButton.setButtonText("Delete");
    deleteButton.onClick = [this] { deleteSelectedPreset(); };
    
    addAndMakeVisible(presetNameEditor);
    presetNameEditor.setText("New Preset");
    
    addAndMakeVisible(nameLabel);
    nameLabel.setText("Name:", juce::dontSendNotification);
    nameLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff0f6fc));
    nameLabel.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    
    addAndMakeVisible(presetLabel);
    presetLabel.setText("Preset:", juce::dontSendNotification);
    presetLabel.attachToComponent(&presetSelector, false);
    presetLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff0f6fc));
    presetLabel.setFont(juce::FontOptions(14.0f, juce::Font::bold));
}

void PresetSection::updatePresetList()
{
    presetSelector.clear();
    auto& presetManager = audioProcessor.getPresetManager();
    auto presets = presetManager.getPresets();
    
    for (int i = 0; i < presets.size(); ++i)
    {
        presetSelector.addItem(presets[i].name, i + 1);
    }
    
    if (presets.size() > 0)
        presetSelector.setSelectedId(1);
}

void PresetSection::saveCurrentPreset()
{
    auto presetName = presetNameEditor.getText();
    if (presetName.isNotEmpty())
    {
        bool isBass = audioProcessor.getInstrumentType() == AlturadspAudioProcessor::Bass;
        audioProcessor.getPresetManager().savePreset(presetName, "User", "User created preset", isBass);
        updatePresetList();
    }
}

void PresetSection::loadSelectedPreset()
{
    int selectedIndex = presetSelector.getSelectedId() - 1;
    if (selectedIndex >= 0)
    {
        auto& presetManager = audioProcessor.getPresetManager();
        auto presets = presetManager.getPresets();
        if (selectedIndex < presets.size())
        {
            presetManager.loadPreset(presets[selectedIndex]);
        }
    }
}

void PresetSection::deleteSelectedPreset()
{
    int selectedIndex = presetSelector.getSelectedId() - 1;
    if (selectedIndex >= 0)
    {
        audioProcessor.getPresetManager().deletePreset(selectedIndex);
        updatePresetList();
    }
}
