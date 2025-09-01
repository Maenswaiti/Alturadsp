#include "AmpSection.h"

AmpSection::AmpSection(AlturadspAudioProcessor& processor) : audioProcessor(processor)
{
    setupControls();
}

void AmpSection::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colour(0xfff5f5f7), 0, 0,
                                 juce::Colour(0xffe5e5e7), 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    auto headerArea = getLocalBounds().removeFromTop(60);
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.fillRoundedRectangle(headerArea.reduced(10).toFloat(), 8.0f);
    
    g.setColour(juce::Colour(0xff1d1d1f));
    g.setFont(juce::FontOptions(20.0f, juce::Font::plain));
    g.drawText("AMP MODELING", headerArea, juce::Justification::centred);
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
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 25);
    
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff007aff));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xffd1d1d6));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xff1d1d1f));
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::white);
    
    addAndMakeVisible(label);
    label.setText(text, juce::dontSendNotification);
    label.attachToComponent(&slider, false);
    label.setColour(juce::Label::textColourId, juce::Colour(0xff1d1d1f));
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::FontOptions(14.0f, juce::Font::bold));
}
