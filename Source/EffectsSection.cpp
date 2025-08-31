#include "EffectsSection.h"

EffectModule::EffectModule(const juce::String& name, const juce::String& description)
    : effectName(name), effectDescription(description)
{
    addAndMakeVisible(enableButton);
    enableButton.setButtonText("");
    enableButton.onClick = [this]() {
        effectEnabled = enableButton.getToggleState();
        repaint();
        if (onEnabledChanged)
            onEnabledChanged(effectEnabled);
    };
}

void EffectModule::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    juce::ColourGradient gradient(effectEnabled ? juce::Colour(0xff1f6feb) : juce::Colour(0xff21262d), 
                                 bounds.getX(), bounds.getY(),
                                 effectEnabled ? juce::Colour(0xff0969da) : juce::Colour(0xff161b22), 
                                 bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 8.0f);
    
    g.setColour(effectEnabled ? juce::Colour(0xff58a6ff) : juce::Colour(0xff30363d));
    g.drawRoundedRectangle(bounds, 8.0f, 2.0f);
    
    if (effectEnabled)
    {
        g.setColour(juce::Colours::black.withAlpha(0.2f));
        g.fillRoundedRectangle(bounds.translated(0, 2), 8.0f);
    }
    
    auto iconBounds = bounds.removeFromTop(40).reduced(8);
    drawEffectIcon(g, iconBounds);
    
    g.setColour(effectEnabled ? juce::Colour(0xfff0f6fc) : juce::Colour(0xff7d8590));
    g.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    g.drawText(effectName, bounds.removeFromTop(20), juce::Justification::centred);
    
    g.setFont(juce::FontOptions(11.0f));
    g.drawText(effectDescription, bounds.removeFromTop(15), juce::Justification::centred);
}

void EffectModule::drawEffectIcon(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    g.setColour(effectEnabled ? juce::Colour(0xff58a6ff) : juce::Colour(0xff7d8590));
    
    if (effectName == "Noise Gate")
    {
        juce::Path gate;
        gate.addRectangle(bounds.reduced(4));
        g.strokePath(gate, juce::PathStrokeType(2.0f));
        g.drawLine(bounds.getX() + 4, bounds.getCentreY(), bounds.getRight() - 4, bounds.getCentreY(), 2.0f);
    }
    else if (effectName == "Compressor")
    {
        juce::Path wave;
        for (int i = 0; i < bounds.getWidth() - 8; i += 2)
        {
            float y = bounds.getCentreY() + std::sin(i * 0.3f) * (bounds.getHeight() * 0.2f);
            if (i == 0) wave.startNewSubPath(bounds.getX() + 4 + i, y);
            else wave.lineTo(bounds.getX() + 4 + i, y);
        }
        g.strokePath(wave, juce::PathStrokeType(2.0f));
    }
    else if (effectName == "Chorus")
    {
        for (int i = 0; i < 3; ++i)
        {
            g.drawEllipse(bounds.getCentreX() - 8 + i * 8, bounds.getCentreY() - 4, 8, 8, 1.5f);
        }
    }
    else if (effectName == "Phaser")
    {
        juce::Path phase;
        for (int i = 0; i < bounds.getWidth() - 8; i += 2)
        {
            float y = bounds.getCentreY() + std::sin(i * 0.2f) * (bounds.getHeight() * 0.3f);
            if (i == 0) phase.startNewSubPath(bounds.getX() + 4 + i, y);
            else phase.lineTo(bounds.getX() + 4 + i, y);
        }
        g.strokePath(phase, juce::PathStrokeType(2.0f));
    }
    else if (effectName == "Delay")
    {
        g.drawEllipse(bounds.reduced(6), 2.0f);
        g.drawLine(bounds.getCentreX() - 8, bounds.getCentreY(), bounds.getCentreX() + 8, bounds.getCentreY(), 2.0f);
        g.drawLine(bounds.getCentreX(), bounds.getCentreY() - 8, bounds.getCentreX(), bounds.getCentreY() + 8, 2.0f);
    }
    else if (effectName == "Reverb")
    {
        for (int i = 0; i < 4; ++i)
        {
            g.drawEllipse(bounds.getCentreX() - 12 + i * 6, bounds.getCentreY() - 6 + i * 2, 12 - i * 2, 12 - i * 2, 1.0f);
        }
    }
    else if (effectName == "Vintage Filter")
    {
        juce::Path filter;
        filter.startNewSubPath(bounds.getX() + 4, bounds.getBottom() - 4);
        filter.lineTo(bounds.getCentreX(), bounds.getY() + 4);
        filter.lineTo(bounds.getRight() - 4, bounds.getBottom() - 4);
        g.strokePath(filter, juce::PathStrokeType(2.0f));
    }
}

void EffectModule::resized()
{
    auto bounds = getLocalBounds();
    enableButton.setBounds(bounds.removeFromTop(20).removeFromRight(30).reduced(2));
    
    if (!controls.isEmpty())
    {
        auto controlArea = bounds.removeFromBottom(bounds.getHeight() - 75);
        int controlWidth = controlArea.getWidth() / controls.size();
        
        for (int i = 0; i < controls.size(); ++i)
        {
            controls[i]->setBounds(controlArea.removeFromLeft(controlWidth).reduced(4));
        }
    }
}

void EffectModule::setEnabled(bool enabled)
{
    effectEnabled = enabled;
    enableButton.setToggleState(enabled, juce::dontSendNotification);
    repaint();
}

void EffectModule::addControl(juce::Component* control, const juce::String& label)
{
    addAndMakeVisible(control);
    controls.add(control);
    controlLabels.add(label);
}

EffectsSection::EffectsSection(AlturadspAudioProcessor& processor) : audioProcessor(processor)
{
    setupEffectModules();
}

void EffectsSection::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colour(0xff161b22), 0, 0,
                                 juce::Colour(0xff21262d), 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    auto headerArea = getLocalBounds().removeFromTop(60);
    g.setColour(juce::Colour(0xff0d1117).withAlpha(0.6f));
    g.fillRoundedRectangle(headerArea.reduced(10).toFloat(), 8.0f);
    
    g.setColour(juce::Colour(0xfff0f6fc));
    g.setFont(juce::FontOptions(20.0f, juce::Font::bold));
    g.drawText("EFFECTS CHAIN", headerArea, juce::Justification::centred);
}

void EffectsSection::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(60);
    
    int moduleWidth = area.getWidth() / 4;
    int moduleHeight = area.getHeight() / 2;
    
    noiseGateModule->setBounds(area.removeFromLeft(moduleWidth).removeFromTop(moduleHeight).reduced(5));
    compressorModule->setBounds(area.removeFromLeft(moduleWidth).removeFromTop(moduleHeight).reduced(5));
    chorusModule->setBounds(area.removeFromLeft(moduleWidth).removeFromTop(moduleHeight).reduced(5));
    phaserModule->setBounds(area.removeFromLeft(moduleWidth).removeFromTop(moduleHeight).reduced(5));
    
    area = getLocalBounds();
    area.removeFromTop(60 + moduleHeight);
    
    delayModule->setBounds(area.removeFromLeft(moduleWidth).removeFromTop(moduleHeight).reduced(5));
    reverbModule->setBounds(area.removeFromLeft(moduleWidth).removeFromTop(moduleHeight).reduced(5));
    vintageFilterModule->setBounds(area.removeFromLeft(moduleWidth).removeFromTop(moduleHeight).reduced(5));
}

void EffectsSection::setupEffectModules()
{
    noiseGateModule = std::make_unique<EffectModule>("Noise Gate", "Silence unwanted noise");
    addAndMakeVisible(noiseGateModule.get());
    
    setupSlider(noiseGateThresholdSlider, "noiseGateThreshold", -60.0f, 0.0f, -40.0f);
    noiseGateModule->addControl(&noiseGateThresholdSlider, "Threshold");
    
    compressorModule = std::make_unique<EffectModule>("Compressor", "Dynamic range control");
    addAndMakeVisible(compressorModule.get());
    
    setupSlider(compressorRatioSlider, "compressorRatio", 1.0f, 20.0f, 4.0f);
    compressorModule->addControl(&compressorRatioSlider, "Ratio");
    
    chorusModule = std::make_unique<EffectModule>("Chorus", "Rich modulation effect");
    addAndMakeVisible(chorusModule.get());
    
    setupSlider(chorusRateSlider, "chorusRate", 0.1f, 5.0f, 0.5f);
    setupSlider(chorusDepthSlider, "chorusDepth", 0.0f, 1.0f, 0.3f);
    chorusModule->addControl(&chorusRateSlider, "Rate");
    chorusModule->addControl(&chorusDepthSlider, "Depth");
    
    phaserModule = std::make_unique<EffectModule>("Phaser", "Sweeping phase effect");
    addAndMakeVisible(phaserModule.get());
    
    setupSlider(phaserRateSlider, "phaserRate", 0.1f, 5.0f, 0.3f);
    setupSlider(phaserDepthSlider, "phaserDepth", 0.0f, 1.0f, 0.4f);
    phaserModule->addControl(&phaserRateSlider, "Rate");
    phaserModule->addControl(&phaserDepthSlider, "Depth");
    
    delayModule = std::make_unique<EffectModule>("Delay", "Echo and repeat effects");
    addAndMakeVisible(delayModule.get());
    
    setupSlider(delayTimeSlider, "delayTime", 10.0f, 1000.0f, 250.0f);
    setupSlider(delayFeedbackSlider, "delayFeedback", 0.0f, 0.9f, 0.3f);
    delayModule->addControl(&delayTimeSlider, "Time");
    delayModule->addControl(&delayFeedbackSlider, "Feedback");
    
    reverbModule = std::make_unique<EffectModule>("Reverb", "Spatial ambience");
    addAndMakeVisible(reverbModule.get());
    
    setupSlider(reverbSizeSlider, "reverbSize", 0.0f, 1.0f, 0.5f);
    setupSlider(reverbDampingSlider, "reverbDamping", 0.0f, 1.0f, 0.5f);
    reverbModule->addControl(&reverbSizeSlider, "Size");
    reverbModule->addControl(&reverbDampingSlider, "Damping");
    
    vintageFilterModule = std::make_unique<EffectModule>("Vintage Filter", "Classic analog filtering");
    addAndMakeVisible(vintageFilterModule.get());
    
    setupSlider(filterCutoffSlider, "filterCutoff", 100.0f, 10000.0f, 2000.0f);
    setupSlider(filterResonanceSlider, "filterResonance", 0.0f, 1.0f, 0.3f);
    vintageFilterModule->addControl(&filterCutoffSlider, "Cutoff");
    vintageFilterModule->addControl(&filterResonanceSlider, "Resonance");
    
    noiseGateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getValueTreeState(), "noiseGateEnabled", noiseGateModule->enableButton);
    compressorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getValueTreeState(), "compressorEnabled", compressorModule->enableButton);
    chorusAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getValueTreeState(), "chorusEnabled", chorusModule->enableButton);
    phaserAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getValueTreeState(), "phaserEnabled", phaserModule->enableButton);
    delayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getValueTreeState(), "delayEnabled", delayModule->enableButton);
    reverbAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getValueTreeState(), "reverbEnabled", reverbModule->enableButton);
    filterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getValueTreeState(), "vintageFilterEnabled", vintageFilterModule->enableButton);
    
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "noiseGateThreshold", noiseGateThresholdSlider);
    ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "compressorRatio", compressorRatioSlider);
    chorusRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "chorusRate", chorusRateSlider);
    chorusDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "chorusDepth", chorusDepthSlider);
    phaserRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "phaserRate", phaserRateSlider);
    phaserDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "phaserDepth", phaserDepthSlider);
    delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "delayTime", delayTimeSlider);
    delayFeedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "delayFeedback", delayFeedbackSlider);
    reverbSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "reverbSize", reverbSizeSlider);
    reverbDampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "reverbDamping", reverbDampingSlider);
    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "filterCutoff", filterCutoffSlider);
    filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getValueTreeState(), "filterResonance", filterResonanceSlider);
}

void EffectsSection::setupSlider(juce::Slider& slider, const juce::String& paramId, float min, float max, float defaultVal)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    slider.setRange(min, max);
    slider.setValue(defaultVal);
    
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff58a6ff));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff30363d));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff0f6fc));
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff21262d));
}
