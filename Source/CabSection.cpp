#include "CabSection.h"

CabSection::CabSection(AlturadspAudioProcessor& processor) : audioProcessor(processor)
{
    setupControls();
}

void CabSection::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff353535));
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    g.drawText("CABINET SIMULATION", getLocalBounds().removeFromTop(40), juce::Justification::centred);
}

void CabSection::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(40);
    
    auto topRow = area.removeFromTop(80);
    cabModelSelector.setBounds(topRow.removeFromLeft(200).reduced(5));
    micTypeSelector.setBounds(topRow.removeFromLeft(200).reduced(5));
    
    auto bottomRow = area.removeFromTop(80);
    micDistanceSlider.setBounds(bottomRow.removeFromLeft(120).reduced(5));
}

void CabSection::setupControls()
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
    
    setupSlider(micDistanceSlider, micDistanceLabel, "Mic Distance");
    
    cabModelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.getValueTreeState(), "cabModel", cabModelSelector);
    micTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.getValueTreeState(), "micType", micTypeSelector);
    micDistanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "micDistance", micDistanceSlider);
}

void CabSection::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text)
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
