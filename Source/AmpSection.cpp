#include "AmpSection.h"

AmpSection::AmpSection(AlturadspAudioProcessor& processor) : audioProcessor(processor)
{
    setupControls();
}

void AmpSection::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff353535));
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    g.drawText("AMP MODELING", getLocalBounds().removeFromTop(40), juce::Justification::centred);
}

void AmpSection::resized()
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

void AmpSection::setupControls()
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

void AmpSection::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text)
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
