#pragma once

#include <JuceHeader.h>

class PresetManager
{
public:
    PresetManager(juce::AudioProcessorValueTreeState& vts);
    ~PresetManager();

    struct Preset
    {
        juce::String name;
        juce::String category;
        juce::String description;
        juce::ValueTree state;
        bool isBassPreset;
    };

    void loadPreset(const Preset& preset);
    void savePreset(const juce::String& name, const juce::String& category, const juce::String& description, bool isBass);
    void deletePreset(int index);
    
    const std::vector<Preset>& getPresets() const { return presets; }
    std::vector<Preset> getPresetsForInstrument(bool isBass) const;
    
    void loadPresetsFromDirectory(const juce::File& directory);
    void savePresetToFile(const Preset& preset, const juce::File& file);
    Preset loadPresetFromFile(const juce::File& file);
    
    void createDefaultPresets();
    
    int getCurrentPresetIndex() const { return currentPresetIndex; }
    void setCurrentPresetIndex(int index) { currentPresetIndex = index; }

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    std::vector<Preset> presets;
    int currentPresetIndex = -1;
    
    void createGuitarPresets();
    void createBassPresets();
    
    Preset createPreset(const juce::String& name, const juce::String& category, const juce::String& description, bool isBass,
                       int ampModel, float ampGain, float bass, float mid, float treble, float presence,
                       int cabModel, int micType, float micDistance,
                       bool noiseGate = false, float gateThreshold = -40.0f,
                       bool compressor = false, float compRatio = 4.0f,
                       bool reverb = false, float reverbSize = 0.5f, float reverbDamping = 0.5f,
                       bool delay = false, float delayTime = 250.0f, float delayFeedback = 0.3f);
};
