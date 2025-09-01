#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AmpSection.h"
#include "CabSection.h"
#include "EffectsSection.h"
#include "PresetSection.h"

AlturadspAudioProcessorEditor::AlturadspAudioProcessorEditor (AlturadspAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), mainTabs(juce::TabbedButtonBar::TabsAtTop)
{
    modernLookAndFeel = std::make_unique<ModernLookAndFeel>();
    setLookAndFeel(modernLookAndFeel.get());
    
    setupUI();
    
    setSize(900, 700);
    startTimerHz(30);
}

AlturadspAudioProcessorEditor::~AlturadspAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    stopTimer();
    removeAllChildren();
}

void AlturadspAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colour(0xfff8f9fa), 0, 0,
                                 juce::Colour(0xfff1f3f4), 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    auto headerArea = getLocalBounds().removeFromTop(80);
    
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.fillRoundedRectangle(headerArea.reduced(8).toFloat(), 8.0f);
    
    g.setColour(juce::Colours::black.withAlpha(0.1f));
    g.fillRoundedRectangle(headerArea.reduced(8).translated(0, 1).toFloat(), 8.0f);
    
    g.setColour(juce::Colour(0xff1d1d1f));
    g.setFont(juce::FontOptions(28.0f, juce::Font::plain));
    g.drawText("ALTURADSP", headerArea, juce::Justification::centred);
    
    g.setColour(juce::Colour(0xff007aff));
    g.fillRect(headerArea.removeFromBottom(2));
}

void AlturadspAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    
    auto headerArea = area.removeFromTop(80);
    
    auto instrumentArea = area.removeFromTop(50);
    instrumentArea = instrumentArea.reduced(20, 10);
    instrumentLabel.setBounds(instrumentArea.removeFromLeft(100));
    instrumentSelector.setBounds(instrumentArea.removeFromLeft(150));
    
    mainTabs.setBounds(area.reduced(10));
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
    
    mainTabs.addTab("Amplifier", juce::Colour(0xfff5f5f7), ampSection.get(), false);
    mainTabs.addTab("Cabinet", juce::Colour(0xfff5f5f7), cabSection.get(), false);
    mainTabs.addTab("Effects", juce::Colour(0xfff5f5f7), effectsSection.get(), false);
    mainTabs.addTab("Presets", juce::Colour(0xfff5f5f7), presetSection.get(), false);
    
    addAndMakeVisible(instrumentSelector);
    instrumentSelector.addItem("Guitar", 1);
    instrumentSelector.addItem("Bass", 2);
    instrumentSelector.setSelectedId(1);
    instrumentSelector.onChange = [this] { updateInstrumentType(); };
    
    addAndMakeVisible(instrumentLabel);
    instrumentLabel.setText("Instrument:", juce::dontSendNotification);
    instrumentLabel.setColour(juce::Label::textColourId, juce::Colour(0xff1d1d1f));
    instrumentLabel.setFont(juce::FontOptions(16.0f, juce::Font::bold));
}

void AlturadspAudioProcessorEditor::updateInstrumentType()
{
    bool isBass = instrumentSelector.getSelectedId() == 2;
    audioProcessor.setInstrumentType(isBass ? AlturadspAudioProcessor::Bass : AlturadspAudioProcessor::Guitar);
}
