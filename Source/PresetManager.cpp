#include "PresetManager.h"

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& vts)
    : valueTreeState(vts)
{
    createDefaultPresets();
}

PresetManager::~PresetManager()
{
}

void PresetManager::loadPreset(const Preset& preset)
{
    valueTreeState.replaceState(preset.state);
    
    if (auto* processor = dynamic_cast<juce::AudioProcessor*>(&valueTreeState.processor))
    {
        processor->updateHostDisplay();
        
        for (auto* param : processor->getParameters())
        {
            if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
            {
                float currentValue = rangedParam->getValue();
                rangedParam->setValueNotifyingHost(currentValue);
                rangedParam->sendValueChangedMessageToListeners(currentValue);
            }
        }
    }
    
    valueTreeState.state.sendPropertyChangeMessage("presetLoaded");
}

void PresetManager::savePreset(const juce::String& name, const juce::String& category, const juce::String& description, bool isBass)
{
    Preset newPreset;
    newPreset.name = name;
    newPreset.category = category;
    newPreset.description = description;
    newPreset.isBassPreset = isBass;
    newPreset.state = valueTreeState.copyState();
    
    presets.push_back(newPreset);
}

void PresetManager::deletePreset(int index)
{
    if (index >= 0 && index < static_cast<int>(presets.size()))
    {
        presets.erase(presets.begin() + index);
        if (currentPresetIndex >= index)
            currentPresetIndex--;
    }
}

std::vector<PresetManager::Preset> PresetManager::getPresetsForInstrument(bool isBass) const
{
    std::vector<Preset> filteredPresets;
    for (const auto& preset : presets)
    {
        if (preset.isBassPreset == isBass)
            filteredPresets.push_back(preset);
    }
    return filteredPresets;
}

void PresetManager::createDefaultPresets()
{
    createGuitarPresets();
    createBassPresets();
}

void PresetManager::createGuitarPresets()
{
    presets.push_back(createPreset("Neural Clean", "Clean", "Crystal clear Neural DSP style clean tone", false,
                                  0, 3.8f, 6.2f, 5.8f, 6.8f, 4.2f, 0, 0, 1.8f, false, -42.0f, true, 2.8f, true, 0.35f, 0.45f));
    
    presets.push_back(createPreset("Vintage Warmth", "Vintage", "Classic tube warmth with character", false,
                                  4, 6.8f, 6.8f, 7.2f, 6.2f, 5.8f, 0, 1, 2.2f, false, -38.0f, false, 4.2f, true, 0.42f, 0.32f));
    
    presets.push_back(createPreset("Modern Crunch", "Crunch", "Tight modern crunch tone", false,
                                  1, 7.2f, 6.5f, 6.8f, 7.2f, 6.5f, 1, 0, 1.5f, true, -36.0f, true, 3.8f, true, 0.38f, 0.28f));
    
    presets.push_back(createPreset("Soaring Lead", "Lead", "Singing sustain lead tone", false,
                                  2, 8.2f, 5.8f, 6.8f, 7.5f, 7.0f, 1, 0, 1.2f, true, -34.0f, true, 3.2f, false, 0.0f, 0.0f, true, 0.45f, 0.35f));
    
    presets.push_back(createPreset("Crushing High Gain", "High Gain", "Modern metal rhythm tone", false,
                                  3, 9.2f, 7.5f, 6.2f, 8.0f, 6.8f, 1, 0, 0.8f, true, -32.0f, true, 4.5f));
    
    presets.push_back(createPreset("Modern Precision", "Modern", "Tight modern high-gain", false,
                                  5, 8.8f, 7.2f, 6.5f, 7.8f, 6.5f, 1, 0, 1.0f, true, -33.0f, true, 4.2f));
    
    presets.push_back(createPreset("Ambient Shimmer", "Clean", "Ethereal ambient clean", false,
                                  0, 4.2f, 6.5f, 6.0f, 7.2f, 5.0f, 0, 2, 2.5f, false, -45.0f, true, 2.0f, true, 0.5f, 0.6f));
    
    presets.push_back(createPreset("Vintage Chorus", "Crunch", "Classic 80s chorus crunch", false,
                                  1, 6.5f, 6.8f, 7.0f, 6.5f, 5.5f, 0, 1, 2.0f, false, -40.0f, false, 4.0f, true, 0.6f, 0.5f));
    
    presets.push_back(createPreset("Country Twang", "Clean", "Classic country clean tone", false,
                                  0, 3.0f, 5.0f, 4.0f, 8.0f, 7.0f, 2, 0, 2.5f));
    
    presets.push_back(createPreset("Blues Crunch", "Crunch", "Classic blues overdrive", false,
                                  1, 6.0f, 6.0f, 7.0f, 5.0f, 4.0f, 0, 0, 2.0f));
    
    presets.push_back(createPreset("Rock Crunch", "Crunch", "Punchy rock rhythm tone", false,
                                  1, 7.0f, 7.0f, 6.0f, 6.0f, 5.0f, 1, 0, 2.0f));
    
    presets.push_back(createPreset("Vintage Crunch", "Crunch", "70s rock crunch tone", false,
                                  4, 6.5f, 6.0f, 7.0f, 5.0f, 4.0f, 0, 3, 3.0f));
    
    presets.push_back(createPreset("Classic Lead", "Lead", "Singing lead guitar tone", false,
                                  2, 8.0f, 5.0f, 7.0f, 6.0f, 6.0f, 0, 0, 1.5f));
    
    presets.push_back(createPreset("Smooth Lead", "Lead", "Creamy lead with sustain", false,
                                  2, 7.5f, 6.0f, 8.0f, 5.0f, 5.0f, 2, 1, 2.0f));
    
    presets.push_back(createPreset("Screaming Lead", "Lead", "High gain lead tone", false,
                                  3, 9.0f, 4.0f, 6.0f, 7.0f, 8.0f, 1, 0, 1.0f));
    
    presets.push_back(createPreset("Metal Rhythm", "High Gain", "Tight metal rhythm", false,
                                  3, 8.5f, 3.0f, 4.0f, 6.0f, 7.0f, 1, 0, 1.0f, true, -35.0f));
    
    presets.push_back(createPreset("Modern Metal", "High Gain", "Contemporary metal tone", false,
                                  5, 9.0f, 4.0f, 5.0f, 7.0f, 8.0f, 1, 0, 1.0f, true, -30.0f));
    
    presets.push_back(createPreset("Djent", "High Gain", "Progressive metal tone", false,
                                  5, 8.0f, 2.0f, 3.0f, 8.0f, 9.0f, 1, 0, 0.5f, true, -25.0f, true, 6.0f));
    
    presets.push_back(createPreset("Ambient Clean", "Clean", "Spacious clean with reverb", false,
                                  0, 2.0f, 5.0f, 5.0f, 6.0f, 4.0f, 2, 2, 3.0f, false, -40.0f, false, 4.0f, true, 0.8f, 0.3f));
    
    presets.push_back(createPreset("Delay Lead", "Lead", "Lead with rhythmic delay", false,
                                  2, 7.0f, 5.0f, 7.0f, 6.0f, 6.0f, 0, 0, 2.0f, false, -40.0f, false, 4.0f, false, 0.5f, 0.5f, true, 375.0f, 0.4f));
    
    presets.push_back(createPreset("Shoegaze", "Lead", "Dreamy shoegaze tone", false,
                                  1, 6.0f, 6.0f, 8.0f, 7.0f, 5.0f, 2, 2, 4.0f, false, -40.0f, false, 4.0f, true, 0.9f, 0.2f, true, 500.0f, 0.6f));
    
    presets.push_back(createPreset("Funk Clean", "Clean", "Percussive funk clean", false,
                                  0, 3.0f, 7.0f, 8.0f, 6.0f, 5.0f, 2, 0, 1.5f, false, -40.0f, true, 8.0f));
    
    presets.push_back(createPreset("Alt Rock", "Crunch", "90s alternative rock", false,
                                  1, 7.0f, 6.0f, 7.0f, 6.0f, 5.0f, 1, 0, 2.0f, false, -40.0f, true, 5.0f));
    
    presets.push_back(createPreset("Indie Rock", "Crunch", "Modern indie rock tone", false,
                                  1, 6.5f, 5.0f, 8.0f, 7.0f, 6.0f, 2, 2, 2.5f, false, -40.0f, false, 4.0f, true, 0.4f, 0.6f));
    
    presets.push_back(createPreset("Stoner Rock", "High Gain", "Heavy stoner rock fuzz", false,
                                  4, 8.0f, 8.0f, 6.0f, 4.0f, 3.0f, 0, 0, 3.0f));
    
    presets.push_back(createPreset("Doom Metal", "High Gain", "Crushing doom tone", false,
                                  4, 9.0f, 9.0f, 5.0f, 3.0f, 2.0f, 0, 0, 4.0f));
    
    presets.push_back(createPreset("Black Metal", "High Gain", "Raw black metal tone", false,
                                  3, 9.5f, 2.0f, 4.0f, 9.0f, 8.0f, 1, 0, 0.5f, true, -20.0f));
    
    presets.push_back(createPreset("Thrash Metal", "High Gain", "Aggressive thrash tone", false,
                                  5, 8.5f, 4.0f, 5.0f, 7.0f, 8.0f, 1, 0, 1.0f, true, -30.0f));
    
    presets.push_back(createPreset("Power Metal", "High Gain", "Epic power metal lead", false,
                                  2, 8.0f, 5.0f, 6.0f, 7.0f, 7.0f, 0, 0, 2.0f, false, -40.0f, false, 4.0f, true, 0.6f, 0.4f));
    
    presets.push_back(createPreset("Post Rock", "Clean", "Atmospheric post rock", false,
                                  0, 2.5f, 5.0f, 6.0f, 7.0f, 5.0f, 2, 2, 3.5f, false, -40.0f, false, 4.0f, true, 0.8f, 0.3f, true, 750.0f, 0.5f));
    
    presets.push_back(createPreset("Surf Rock", "Clean", "Classic surf guitar", false,
                                  0, 3.0f, 4.0f, 5.0f, 8.0f, 6.0f, 2, 0, 2.0f, false, -40.0f, false, 4.0f, true, 0.5f, 0.7f, true, 200.0f, 0.2f));
}

void PresetManager::createBassPresets()
{
    presets.push_back(createPreset("Jazz Bass", "Clean", "Warm jazz bass tone", true,
                                  0, 2.0f, 7.0f, 6.0f, 4.0f, 3.0f, 4, 0, 2.0f));
    
    presets.push_back(createPreset("Funk Bass", "Clean", "Punchy funk bass", true,
                                  0, 3.0f, 6.0f, 8.0f, 5.0f, 4.0f, 5, 0, 1.5f, false, -40.0f, true, 6.0f));
    
    presets.push_back(createPreset("Rock Bass", "Crunch", "Classic rock bass", true,
                                  1, 5.0f, 7.0f, 6.0f, 5.0f, 4.0f, 4, 0, 2.0f));
    
    presets.push_back(createPreset("Metal Bass", "High Gain", "Heavy metal bass", true,
                                  3, 7.0f, 5.0f, 4.0f, 6.0f, 5.0f, 4, 0, 1.0f, true, -35.0f));
    
    presets.push_back(createPreset("Reggae Bass", "Clean", "Deep reggae bass", true,
                                  0, 2.5f, 8.0f, 5.0f, 3.0f, 2.0f, 4, 0, 3.0f));
    
    presets.push_back(createPreset("Slap Bass", "Clean", "Bright slap bass", true,
                                  0, 3.5f, 5.0f, 7.0f, 7.0f, 6.0f, 5, 0, 1.5f, false, -40.0f, true, 8.0f));
    
    presets.push_back(createPreset("Dub Bass", "Clean", "Deep dub bass", true,
                                  0, 2.0f, 9.0f, 4.0f, 2.0f, 1.0f, 4, 0, 4.0f, false, -40.0f, false, 4.0f, true, 0.7f, 0.8f));
    
    presets.push_back(createPreset("Prog Bass", "Crunch", "Progressive bass tone", true,
                                  1, 6.0f, 6.0f, 7.0f, 6.0f, 5.0f, 5, 0, 2.0f, false, -40.0f, true, 4.0f));
    
    presets.push_back(createPreset("Punk Bass", "Crunch", "Aggressive punk bass", true,
                                  1, 7.0f, 6.0f, 6.0f, 6.0f, 5.0f, 4, 0, 1.5f));
    
    presets.push_back(createPreset("Alternative Bass", "Crunch", "90s alternative bass", true,
                                  1, 5.5f, 7.0f, 7.0f, 5.0f, 4.0f, 5, 0, 2.0f, false, -40.0f, true, 5.0f));
    
    presets.push_back(createPreset("Doom Bass", "High Gain", "Heavy doom bass", true,
                                  4, 8.0f, 8.0f, 5.0f, 3.0f, 2.0f, 4, 0, 3.0f));
    
    presets.push_back(createPreset("Death Metal Bass", "High Gain", "Brutal death metal bass", true,
                                  5, 8.5f, 4.0f, 4.0f, 6.0f, 5.0f, 4, 0, 1.0f, true, -30.0f));
    
    presets.push_back(createPreset("Fretless Bass", "Clean", "Smooth fretless tone", true,
                                  0, 2.5f, 7.0f, 7.0f, 4.0f, 3.0f, 4, 1, 2.5f, false, -40.0f, false, 4.0f, true, 0.3f, 0.6f));
    
    presets.push_back(createPreset("Upright Bass", "Clean", "Acoustic upright simulation", true,
                                  0, 1.5f, 8.0f, 6.0f, 3.0f, 2.0f, 4, 3, 4.0f));
    
    presets.push_back(createPreset("Synth Bass", "Clean", "Synthesizer-like bass", true,
                                  0, 4.0f, 6.0f, 8.0f, 6.0f, 5.0f, 5, 0, 1.0f, false, -40.0f, true, 10.0f));
    
    presets.push_back(createPreset("Motown Bass", "Clean", "Classic Motown bass", true,
                                  0, 3.0f, 7.0f, 7.0f, 4.0f, 3.0f, 4, 0, 2.5f, false, -40.0f, true, 4.0f));
    
    presets.push_back(createPreset("Country Bass", "Clean", "Country bass tone", true,
                                  0, 2.5f, 6.0f, 6.0f, 5.0f, 4.0f, 5, 0, 2.0f));
    
    presets.push_back(createPreset("Latin Bass", "Clean", "Latin music bass", true,
                                  0, 3.0f, 7.0f, 7.0f, 5.0f, 4.0f, 4, 0, 2.0f, false, -40.0f, false, 4.0f, true, 0.4f, 0.5f));
    
    presets.push_back(createPreset("Ambient Bass", "Clean", "Atmospheric bass pad", true,
                                  0, 2.0f, 8.0f, 5.0f, 3.0f, 2.0f, 4, 2, 4.0f, false, -40.0f, false, 4.0f, true, 0.9f, 0.2f, true, 1000.0f, 0.7f));
    
    presets.push_back(createPreset("Distorted Bass", "High Gain", "Heavily distorted bass", true,
                                  3, 8.0f, 6.0f, 5.0f, 5.0f, 4.0f, 4, 0, 1.5f, true, -35.0f));
    
    presets.push_back(createPreset("Grunge Bass", "Crunch", "90s grunge bass", true,
                                  1, 6.5f, 7.0f, 6.0f, 5.0f, 4.0f, 4, 0, 2.0f));
    
    presets.push_back(createPreset("Stoner Bass", "High Gain", "Stoner rock bass", true,
                                  4, 7.5f, 8.0f, 5.0f, 4.0f, 3.0f, 4, 0, 3.0f));
    
    presets.push_back(createPreset("Post-Punk Bass", "Crunch", "Post-punk bass tone", true,
                                  1, 6.0f, 6.0f, 7.0f, 6.0f, 5.0f, 5, 0, 2.0f, false, -40.0f, false, 4.0f, true, 0.5f, 0.6f));
    
    presets.push_back(createPreset("Hardcore Bass", "High Gain", "Hardcore punk bass", true,
                                  3, 8.0f, 5.0f, 5.0f, 6.0f, 5.0f, 4, 0, 1.0f, true, -30.0f));
    
    presets.push_back(createPreset("Indie Bass", "Clean", "Indie rock bass", true,
                                  0, 3.0f, 6.0f, 7.0f, 6.0f, 5.0f, 5, 2, 2.0f, false, -40.0f, false, 4.0f, true, 0.4f, 0.5f));
    
    presets.push_back(createPreset("Psychedelic Bass", "Crunch", "Psychedelic bass tone", true,
                                  1, 5.0f, 7.0f, 8.0f, 5.0f, 4.0f, 4, 2, 3.0f, false, -40.0f, false, 4.0f, true, 0.7f, 0.4f, true, 600.0f, 0.5f));
}

PresetManager::Preset PresetManager::createPreset(const juce::String& name, const juce::String& category, const juce::String& description, bool isBass,
                                                 int ampModel, float ampGain, float bass, float mid, float treble, float presence,
                                                 int cabModel, int micType, float micDistance,
                                                 bool noiseGate, float gateThreshold,
                                                 bool compressor, float compRatio,
                                                 bool reverb, float reverbSize, float reverbDamping,
                                                 bool delay, float delayTime, float delayFeedback)
{
    Preset preset;
    preset.name = name;
    preset.category = category;
    preset.description = description;
    preset.isBassPreset = isBass;
    
    juce::ValueTree state("Parameters");
    state.setProperty("ampModel", ampModel, nullptr);
    state.setProperty("ampGain", ampGain, nullptr);
    state.setProperty("ampBass", bass, nullptr);
    state.setProperty("ampMid", mid, nullptr);
    state.setProperty("ampTreble", treble, nullptr);
    state.setProperty("ampPresence", presence, nullptr);
    state.setProperty("cabModel", cabModel, nullptr);
    state.setProperty("micType", micType, nullptr);
    state.setProperty("micDistance", micDistance, nullptr);
    state.setProperty("noiseGateEnable", noiseGate, nullptr);
    state.setProperty("noiseGateThreshold", gateThreshold, nullptr);
    state.setProperty("compressorEnable", compressor, nullptr);
    state.setProperty("compressorRatio", compRatio, nullptr);
    state.setProperty("reverbEnable", reverb, nullptr);
    state.setProperty("reverbSize", reverbSize, nullptr);
    state.setProperty("reverbDamping", reverbDamping, nullptr);
    state.setProperty("delayEnable", delay, nullptr);
    state.setProperty("delayTime", delayTime, nullptr);
    state.setProperty("delayFeedback", delayFeedback, nullptr);
    
    state.setProperty("chorusEnable", false, nullptr);
    state.setProperty("chorusRate", 0.5f, nullptr);
    state.setProperty("chorusDepth", 0.3f, nullptr);
    state.setProperty("phaserEnable", false, nullptr);
    state.setProperty("phaserRate", 0.3f, nullptr);
    state.setProperty("phaserDepth", 0.4f, nullptr);
    state.setProperty("vintageFilterEnable", false, nullptr);
    state.setProperty("filterCutoff", 2000.0f, nullptr);
    state.setProperty("filterResonance", 0.3f, nullptr);
    
    preset.state = state;
    return preset;
}
