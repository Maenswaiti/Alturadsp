#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AmpSection.h"
#include "CabSection.h"
#include "EffectsSection.h"
#include "PresetSection.h"

AlturadspAudioProcessorEditor::AlturadspAudioProcessorEditor (AlturadspAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), mainTabs(juce::TabbedButtonBar::TabsAtTop)
{
    backgroundColour = juce::Colour(0xff2a2a2a);
    
    setupUI();
    
    setSize(800, 600);
    startTimerHz(30);
}

AlturadspAudioProcessorEditor::~AlturadspAudioProcessorEditor()
{
    stopTimer();
}

void AlturadspAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(backgroundColour);
    
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    g.drawText("ALTURADSP", getLocalBounds().removeFromTop(50), juce::Justification::centred);
}

void AlturadspAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    
    auto headerArea = area.removeFromTop(50);
    
    auto instrumentArea = area.removeFromTop(40);
    instrumentLabel.setBounds(instrumentArea.removeFromLeft(100));
    instrumentSelector.setBounds(instrumentArea.removeFromLeft(150).reduced(5));
    
    mainTabs.setBounds(area);
}

void AlturadspAudioProcessorEditor::timerCallback()
{
}

void AlturadspAudioProcessorEditor::setupUI()
{
    addAndMakeVisible(mainTabs);
    
    ampSection = std::make_unique<AmpSection>(audioProcessor);
    cabSection = std::make_unique<CabSection>(audioProcessor);
    effectsSection = std::make_unique<EffectsSection>(audioProcessor);
    presetSection = std::make_unique<PresetSection>(audioProcessor);
    
    mainTabs.addTab("Amp", juce::Colour(0xff404040), ampSection.get(), false);
    mainTabs.addTab("Cabinet", juce::Colour(0xff404040), cabSection.get(), false);
    mainTabs.addTab("Effects", juce::Colour(0xff404040), effectsSection.get(), false);
    mainTabs.addTab("Presets", juce::Colour(0xff404040), presetSection.get(), false);
    
    addAndMakeVisible(instrumentSelector);
    instrumentSelector.addItem("Guitar", 1);
    instrumentSelector.addItem("Bass", 2);
    instrumentSelector.setSelectedId(1);
    instrumentSelector.onChange = [this] { updateInstrumentType(); };
    
    addAndMakeVisible(instrumentLabel);
    instrumentLabel.setText("Instrument:", juce::dontSendNotification);
    instrumentLabel.setColour(juce::Label::textColourId, juce::Colours::white);
}

void AlturadspAudioProcessorEditor::updateInstrumentType()
{
    bool isBass = instrumentSelector.getSelectedId() == 2;
    audioProcessor.setInstrumentType(isBass ? AlturadspAudioProcessor::Bass : AlturadspAudioProcessor::Guitar);
}
