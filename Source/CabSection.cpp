#include "CabSection.h"

CabSection::CabSection(AlturadspAudioProcessor& processor) : audioProcessor(processor)
{
    setupControls();
}

void CabSection::paint(juce::Graphics& g)
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
    g.drawText("CABINET SIMULATION", headerArea, juce::Justification::centred);
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
