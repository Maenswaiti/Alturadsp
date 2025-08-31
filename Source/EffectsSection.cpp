#include "EffectsSection.h"

EffectsSection::EffectsSection(AlturadspAudioProcessor& processor) : audioProcessor(processor)
{
    setupControls();
}

void EffectsSection::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff353535));
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    g.drawText("EFFECTS CHAIN", getLocalBounds().removeFromTop(40), juce::Justification::centred);
}

void EffectsSection::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(40);
    
    auto row1 = area.removeFromTop(100);
    auto noiseGateArea = row1.removeFromLeft(200);
    noiseGateToggle.setBounds(noiseGateArea.removeFromTop(30));
    noiseGateThresholdSlider.setBounds(noiseGateArea.reduced(5));
    
    auto compressorArea = row1.removeFromLeft(200);
    compressorToggle.setBounds(compressorArea.removeFromTop(30));
    compressorRatioSlider.setBounds(compressorArea.reduced(5));
    
    auto row2 = area.removeFromTop(100);
    auto reverbArea = row2.removeFromLeft(200);
    reverbToggle.setBounds(reverbArea.removeFromTop(30));
    auto reverbSliders = reverbArea.reduced(5);
    reverbSizeSlider.setBounds(reverbSliders.removeFromLeft(90));
    reverbDampingSlider.setBounds(reverbSliders);
    
    auto delayArea = row2.removeFromLeft(200);
    delayToggle.setBounds(delayArea.removeFromTop(30));
    auto delaySliders = delayArea.reduced(5);
    delayTimeSlider.setBounds(delaySliders.removeFromLeft(90));
    delayFeedbackSlider.setBounds(delaySliders);
}

void EffectsSection::setupControls()
{
    addAndMakeVisible(noiseGateToggle);
    noiseGateToggle.setButtonText("Noise Gate");
    noiseGateToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    
    addAndMakeVisible(compressorToggle);
    compressorToggle.setButtonText("Compressor");
    compressorToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    
    addAndMakeVisible(reverbToggle);
    reverbToggle.setButtonText("Reverb");
    reverbToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    
    addAndMakeVisible(delayToggle);
    delayToggle.setButtonText("Delay");
    delayToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    
    setupSlider(noiseGateThresholdSlider, thresholdLabel, "Threshold");
    setupSlider(compressorRatioSlider, ratioLabel, "Ratio");
    setupSlider(reverbSizeSlider, sizeLabel, "Size");
    setupSlider(reverbDampingSlider, dampingLabel, "Damping");
    setupSlider(delayTimeSlider, timeLabel, "Time");
    setupSlider(delayFeedbackSlider, feedbackLabel, "Feedback");
    
    noiseGateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getValueTreeState(), "noiseGateEnable", noiseGateToggle);
    compressorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getValueTreeState(), "compressorEnable", compressorToggle);
    reverbAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getValueTreeState(), "reverbEnable", reverbToggle);
    delayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getValueTreeState(), "delayEnable", delayToggle);
    
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "noiseGateThreshold", noiseGateThresholdSlider);
    ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "compressorRatio", compressorRatioSlider);
    sizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "reverbSize", reverbSizeSlider);
    dampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "reverbDamping", reverbDampingSlider);
    timeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "delayTime", delayTimeSlider);
    feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "delayFeedback", delayFeedbackSlider);
}

void EffectsSection::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 15);
    
    addAndMakeVisible(label);
    label.setText(text, juce::dontSendNotification);
    label.attachToComponent(&slider, false);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
}
