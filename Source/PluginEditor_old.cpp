#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AmpSection.h"
#include "CabSection.h"
#include "EffectsSection.h"
#include "PresetSection.h"

AlturadspAudioProcessorEditor::AlturadspAudioProcessorEditor (AlturadspAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), mainTabs(juce::TabbedButtonBar::TabsAtTop)
{
    setSize (800, 600);
    setupUI();
    startTimerHz(30);
}

AlturadspAudioProcessorEditor::~AlturadspAudioProcessorEditor()
{
}

void AlturadspAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (backgroundColour);
    
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (24.0f, juce::Font::bold));
    g.drawFittedText ("ALTURADSP", getLocalBounds().removeFromTop(50), juce::Justification::centred, 1);
}

void AlturadspAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    
    auto headerArea = area.removeFromTop(60);
    auto instrumentArea = headerArea.removeFromRight(200).reduced(10);
    
    instrumentLabel.setBounds(instrumentArea.removeFromTop(20));
    instrumentSelector.setBounds(instrumentArea);
    
    mainTabs.setBounds(area.reduced(10));
}

void AlturadspAudioProcessorEditor::timerCallback()
{
    repaint();
}

void AlturadspAudioProcessorEditor::setupUI()
{
    backgroundColour = juce::Colour(0xff2a2a2a);
    
    addAndMakeVisible(instrumentLabel);
    instrumentLabel.setText("Instrument:", juce::dontSendNotification);
    instrumentLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    
    addAndMakeVisible(instrumentSelector);
    instrumentSelector.addItem("Guitar", 1);
    instrumentSelector.addItem("Bass", 2);
    instrumentSelector.setSelectedId(1);
    instrumentSelector.onChange = [this] { updateInstrumentType(); };
    
    ampSection = std::make_unique<AmpSection>(audioProcessor);
    cabSection = std::make_unique<CabSection>(audioProcessor);
    effectsSection = std::make_unique<EffectsSection>(audioProcessor);
    presetSection = std::make_unique<PresetSection>(audioProcessor);
    
    addAndMakeVisible(mainTabs);
    mainTabs.addTab("Amp", juce::Colour(0xff404040), ampSection.get(), false);
    mainTabs.addTab("Cabinet", juce::Colour(0xff404040), cabSection.get(), false);
    mainTabs.addTab("Effects", juce::Colour(0xff404040), effectsSection.get(), false);
    mainTabs.addTab("Presets", juce::Colour(0xff404040), presetSection.get(), false);
    
    mainTabs.setTabBarDepth(40);
    mainTabs.setCurrentTabIndex(0);
}

void AlturadspAudioProcessorEditor::updateInstrumentType()
{
    bool isBass = instrumentSelector.getSelectedId() == 2;
    audioProcessor.setInstrumentType(isBass ? AlturadspAudioProcessor::Bass : AlturadspAudioProcessor::Guitar);
}

class AlturadspAudioProcessorEditor::AmpSection : public juce::Component
{
public:
    AmpSection(AlturadspAudioProcessor& processor) : audioProcessor(processor)
    {
        setupControls();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff353535));
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
        g.drawText("AMP MODELING", getLocalBounds().removeFromTop(40), juce::Justification::centred);
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(20);
        area.removeFromTop(40);
        
        auto topRow = area.removeFromTop(80);
        ampModelSelector.setBounds(topRow.removeFromLeft(200).reduced(5));
        gainSlider.setBounds(topRow.removeFromLeft(120).reduced(5));
        
        auto bottomRow = area.removeFromTop(80);
        bassSlider.setBounds(bottomRow.removeFromLeft(120).reduced(5));
        midSlider.setBounds(bottomRow.removeFromLeft(120).reduced(5));
        trebleSlider.setBounds(bottomRow.removeFromLeft(120).reduced(5));
        presenceSlider.setBounds(bottomRow.removeFromLeft(120).reduced(5));
    }
    
private:
    AlturadspAudioProcessor& audioProcessor;
    juce::ComboBox ampModelSelector;
    juce::Slider gainSlider, bassSlider, midSlider, trebleSlider, presenceSlider;
    juce::Label ampModelLabel, gainLabel, bassLabel, midLabel, trebleLabel, presenceLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ampModelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment, bassAttachment, midAttachment, trebleAttachment, presenceAttachment;
    
    void setupControls()
    {
        addAndMakeVisible(ampModelSelector);
        ampModelSelector.addItem("Clean", 1);
        ampModelSelector.addItem("Crunch", 2);
        ampModelSelector.addItem("Lead", 3);
        ampModelSelector.addItem("High Gain", 4);
        ampModelSelector.addItem("Vintage", 5);
        ampModelSelector.addItem("Modern", 6);
        
        setupSlider(gainSlider, gainLabel, "Gain");
        setupSlider(bassSlider, bassLabel, "Bass");
        setupSlider(midSlider, midLabel, "Mid");
        setupSlider(trebleSlider, trebleLabel, "Treble");
        setupSlider(presenceSlider, presenceLabel, "Presence");
        
        ampModelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.getValueTreeState(), "ampModel", ampModelSelector);
        gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "ampGain", gainSlider);
        bassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "ampBass", bassSlider);
        midAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "ampMid", midSlider);
        trebleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "ampTreble", trebleSlider);
        presenceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "ampPresence", presenceSlider);
    }
    
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text)
    {
        addAndMakeVisible(slider);
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        
        addAndMakeVisible(label);
        label.setText(text, juce::dontSendNotification);
        label.attachToComponent(&slider, false);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        label.setJustificationType(juce::Justification::centred);
    }
};

class AlturadspAudioProcessorEditor::CabSection : public juce::Component
{
public:
    CabSection(AlturadspAudioProcessor& processor) : audioProcessor(processor)
    {
        setupControls();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff353535));
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
        g.drawText("CABINET SIMULATION", getLocalBounds().removeFromTop(40), juce::Justification::centred);
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(20);
        area.removeFromTop(40);
        
        auto topRow = area.removeFromTop(80);
        cabModelSelector.setBounds(topRow.removeFromLeft(200).reduced(5));
        micTypeSelector.setBounds(topRow.removeFromLeft(200).reduced(5));
        micDistanceSlider.setBounds(topRow.removeFromLeft(120).reduced(5));
    }
    
private:
    AlturadspAudioProcessor& audioProcessor;
    juce::ComboBox cabModelSelector, micTypeSelector;
    juce::Slider micDistanceSlider;
    juce::Label cabModelLabel, micTypeLabel, micDistanceLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> cabModelAttachment, micTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> micDistanceAttachment;
    
    void setupControls()
    {
        addAndMakeVisible(cabModelSelector);
        cabModelSelector.addItem("4x12 Vintage", 1);
        cabModelSelector.addItem("4x12 Modern", 2);
        cabModelSelector.addItem("2x12 Combo", 3);
        cabModelSelector.addItem("1x12 Studio", 4);
        cabModelSelector.addItem("Bass 8x10", 5);
        cabModelSelector.addItem("Bass 4x10", 6);
        
        addAndMakeVisible(micTypeSelector);
        micTypeSelector.addItem("Dynamic 57", 1);
        micTypeSelector.addItem("Dynamic 421", 2);
        micTypeSelector.addItem("Condenser 414", 3);
        micTypeSelector.addItem("Ribbon 121", 4);
        
        addAndMakeVisible(micDistanceSlider);
        micDistanceSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        micDistanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        
        addAndMakeVisible(cabModelLabel);
        cabModelLabel.setText("Cabinet", juce::dontSendNotification);
        cabModelLabel.attachToComponent(&cabModelSelector, false);
        cabModelLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        cabModelLabel.setJustificationType(juce::Justification::centred);
        
        addAndMakeVisible(micTypeLabel);
        micTypeLabel.setText("Microphone", juce::dontSendNotification);
        micTypeLabel.attachToComponent(&micTypeSelector, false);
        micTypeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        micTypeLabel.setJustificationType(juce::Justification::centred);
        
        addAndMakeVisible(micDistanceLabel);
        micDistanceLabel.setText("Distance", juce::dontSendNotification);
        micDistanceLabel.attachToComponent(&micDistanceSlider, false);
        micDistanceLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        micDistanceLabel.setJustificationType(juce::Justification::centred);
        
        cabModelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.getValueTreeState(), "cabModel", cabModelSelector);
        micTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.getValueTreeState(), "micType", micTypeSelector);
        micDistanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "micDistance", micDistanceSlider);
    }
};

class AlturadspAudioProcessorEditor::EffectsSection : public juce::Component
{
public:
    EffectsSection(AlturadspAudioProcessor& processor) : audioProcessor(processor)
    {
        setupControls();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff353535));
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
        g.drawText("EFFECTS CHAIN", getLocalBounds().removeFromTop(40), juce::Justification::centred);
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(20);
        area.removeFromTop(40);
        
        auto row1 = area.removeFromTop(100);
        setupEffectRow(row1, noiseGateToggle, gateThresholdSlider, "GATE");
        
        auto row2 = area.removeFromTop(100);
        setupEffectRow(row2, compressorToggle, compRatioSlider, "COMP");
        
        auto row3 = area.removeFromTop(100);
        setupEffectRow(row3, reverbToggle, reverbSizeSlider, "REVERB");
        
        auto row4 = area.removeFromTop(100);
        setupEffectRow(row4, delayToggle, delayTimeSlider, "DELAY");
    }
    
private:
    AlturadspAudioProcessor& audioProcessor;
    
    juce::ToggleButton noiseGateToggle, compressorToggle, reverbToggle, delayToggle;
    juce::Slider gateThresholdSlider, compRatioSlider, reverbSizeSlider, delayTimeSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> gateAttachment, compAttachment, reverbAttachment, delayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gateThresholdAttachment, compRatioAttachment, reverbSizeAttachment, delayTimeAttachment;
    
    void setupControls()
    {
        setupEffect(noiseGateToggle, gateThresholdSlider, "noiseGateEnable", "noiseGateThreshold", gateAttachment, gateThresholdAttachment);
        setupEffect(compressorToggle, compRatioSlider, "compressorEnable", "compressorRatio", compAttachment, compRatioAttachment);
        setupEffect(reverbToggle, reverbSizeSlider, "reverbEnable", "reverbSize", reverbAttachment, reverbSizeAttachment);
        setupEffect(delayToggle, delayTimeSlider, "delayEnable", "delayTime", delayAttachment, delayTimeAttachment);
    }
    
    void setupEffect(juce::ToggleButton& toggle, juce::Slider& slider, const juce::String& toggleParam, const juce::String& sliderParam,
                    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>& toggleAttachment,
                    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& sliderAttachment)
    {
        addAndMakeVisible(toggle);
        addAndMakeVisible(slider);
        
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        
        toggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getValueTreeState(), toggleParam, toggle);
        sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), sliderParam, slider);
    }
    
    void setupEffectRow(juce::Rectangle<int> area, juce::ToggleButton& toggle, juce::Slider& slider, const juce::String& name)
    {
        auto toggleArea = area.removeFromLeft(80);
        toggle.setBounds(toggleArea.reduced(10));
        toggle.setButtonText(name);
        toggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
        
        slider.setBounds(area.removeFromLeft(120).reduced(10));
    }
};

class AlturadspAudioProcessorEditor::PresetSection : public juce::Component
{
public:
    PresetSection(AlturadspAudioProcessor& processor) : audioProcessor(processor)
    {
        setupControls();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff353535));
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
        g.drawText("PRESET MANAGER", getLocalBounds().removeFromTop(40), juce::Justification::centred);
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(20);
        area.removeFromTop(40);
        
        auto topRow = area.removeFromTop(40);
        categoryFilter.setBounds(topRow.removeFromLeft(150).reduced(5));
        
        auto listArea = area.removeFromTop(300);
        presetList.setBounds(listArea);
        
        auto buttonRow = area.removeFromTop(40);
        loadButton.setBounds(buttonRow.removeFromLeft(80).reduced(5));
        saveButton.setBounds(buttonRow.removeFromLeft(80).reduced(5));
        deleteButton.setBounds(buttonRow.removeFromLeft(80).reduced(5));
    }
    
private:
    AlturadspAudioProcessor& audioProcessor;
    
    juce::ComboBox categoryFilter;
    juce::ListBox presetList;
    juce::TextButton loadButton, saveButton, deleteButton;
    
    void setupControls()
    {
        addAndMakeVisible(categoryFilter);
        categoryFilter.addItem("All", 1);
        categoryFilter.addItem("Clean", 2);
        categoryFilter.addItem("Crunch", 3);
        categoryFilter.addItem("Lead", 4);
        categoryFilter.addItem("High Gain", 5);
        categoryFilter.setSelectedId(1);
        
        addAndMakeVisible(presetList);
        
        addAndMakeVisible(loadButton);
        loadButton.setButtonText("Load");
        loadButton.onClick = [this] { loadSelectedPreset(); };
        
        addAndMakeVisible(saveButton);
        saveButton.setButtonText("Save");
        saveButton.onClick = [this] { saveCurrentPreset(); };
        
        addAndMakeVisible(deleteButton);
        deleteButton.setButtonText("Delete");
        deleteButton.onClick = [this] { deleteSelectedPreset(); };
    }
    
    void loadSelectedPreset()
    {
    }
    
    void saveCurrentPreset()
    {
    }
    
    void deleteSelectedPreset()
    {
    }
};
