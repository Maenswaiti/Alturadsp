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
    
    backgroundColour = juce::Colour(0xff1a1a1a);
    
    setupUI();
    
    setSize(900, 700);
    startTimerHz(30);
}

AlturadspAudioProcessorEditor::~AlturadspAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    stopTimer();
}

void AlturadspAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colour(0xff1a1a1a), 0, 0,
                                 juce::Colour(0xff2d2d2d), 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    auto headerArea = getLocalBounds().removeFromTop(80);
    g.setColour(juce::Colour(0xff0d1117).withAlpha(0.8f));
    g.fillRoundedRectangle(headerArea.toFloat(), 12.0f);
    
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillRoundedRectangle(headerArea.translated(0, 2).toFloat(), 12.0f);
    
    g.setColour(juce::Colour(0xfff0f6fc));
    g.setFont(juce::FontOptions(32.0f, juce::Font::bold));
    g.drawText("ALTURADSP", headerArea, juce::Justification::centred);
    
    g.setColour(juce::Colour(0xff58a6ff));
    g.fillRect(headerArea.removeFromBottom(3));
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
    
    mainTabs.addTab("Amp", juce::Colour(0xff21262d), ampSection.get(), false);
    mainTabs.addTab("Cabinet", juce::Colour(0xff21262d), cabSection.get(), false);
    mainTabs.addTab("Effects", juce::Colour(0xff21262d), effectsSection.get(), false);
    mainTabs.addTab("Presets", juce::Colour(0xff21262d), presetSection.get(), false);
    
    addAndMakeVisible(instrumentSelector);
    instrumentSelector.addItem("Guitar", 1);
    instrumentSelector.addItem("Bass", 2);
    instrumentSelector.setSelectedId(1);
    instrumentSelector.onChange = [this] { updateInstrumentType(); };
    
    addAndMakeVisible(instrumentLabel);
    instrumentLabel.setText("Instrument:", juce::dontSendNotification);
    instrumentLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff0f6fc));
    instrumentLabel.setFont(juce::FontOptions(16.0f, juce::Font::bold));
}

void AlturadspAudioProcessorEditor::updateInstrumentType()
{
    bool isBass = instrumentSelector.getSelectedId() == 2;
    audioProcessor.setInstrumentType(isBass ? AlturadspAudioProcessor::Bass : AlturadspAudioProcessor::Guitar);
}
