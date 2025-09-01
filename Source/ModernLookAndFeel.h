#pragma once
#include <JuceHeader.h>

class ModernLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernLookAndFeel()
    {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xfff5f5f7));
        setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
        setColour(juce::ComboBox::textColourId, juce::Colour(0xff1d1d1f));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xffd1d1d6));
        setColour(juce::ComboBox::arrowColourId, juce::Colour(0xff007aff));
        
        setColour(juce::ToggleButton::textColourId, juce::Colour(0xff1d1d1f));
        setColour(juce::ToggleButton::tickColourId, juce::Colour(0xff007aff));
        setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xffd1d1d6));
        
        // Enhanced tab styling for better visibility
        setColour(juce::TabbedComponent::backgroundColourId, juce::Colour(0xfff5f5f7));
        setColour(juce::TabbedComponent::outlineColourId, juce::Colour(0xffd1d1d6));
        setColour(juce::TabbedButtonBar::tabOutlineColourId, juce::Colour(0xffd1d1d6));
        setColour(juce::TabbedButtonBar::frontOutlineColourId, juce::Colour(0xff007aff));
        setColour(juce::TabbedButtonBar::tabTextColourId, juce::Colour(0xff1d1d1f));
        setColour(juce::TabbedButtonBar::frontTextColourId, juce::Colour(0xff007aff));
        
        // Additional component styling for comprehensive Apple design
        setColour(juce::TextButton::buttonColourId, juce::Colours::white);
        setColour(juce::TextButton::textColourOffId, juce::Colour(0xff1d1d1f));
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff007aff));
        
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff007aff));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xffd1d1d6));
        setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xff1d1d1f));
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::white);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xffd1d1d6));
        
        setColour(juce::Label::textColourId, juce::Colour(0xff1d1d1f));
        setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    }
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(10);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centre = bounds.getCentre();
        
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.fillEllipse(centre.x - radius + 2, centre.y - radius + 2, radius * 2, radius * 2);
        
        juce::ColourGradient gradient(juce::Colours::white, centre.x, centre.y - radius,
                                     juce::Colour(0xfff0f0f0), centre.x, centre.y + radius, false);
        g.setGradientFill(gradient);
        g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2, radius * 2);
        
        g.setColour(juce::Colour(0xff007aff));
        g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2, radius * 2, 2.0f);
        
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineLength = radius * 0.7f;
        auto lineThickness = 3.0f;
        
        g.setColour(juce::Colour(0xff007aff));
        g.drawLine(centre.x, centre.y, 
                  centre.x + std::cos(angle - juce::MathConstants<float>::halfPi) * lineLength,
                  centre.y + std::sin(angle - juce::MathConstants<float>::halfPi) * lineLength,
                  lineThickness);
    }
    
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        auto toggleBounds = bounds.removeFromLeft(bounds.getHeight()).reduced(2);
        
        if (button.getToggleState())
        {
            juce::ColourGradient gradient(juce::Colour(0xff007aff), toggleBounds.getX(), toggleBounds.getY(),
                                         juce::Colour(0xff0051d5), toggleBounds.getRight(), toggleBounds.getBottom(), false);
            g.setGradientFill(gradient);
        }
        else
        {
            g.setColour(juce::Colour(0xffd1d1d6));
        }
        
        g.fillRoundedRectangle(toggleBounds, 4.0f);
        
        g.setColour(juce::Colour(0xffc7c7cc));
        g.drawRoundedRectangle(toggleBounds, 4.0f, 1.0f);
        
        if (button.getToggleState())
        {
            g.setColour(juce::Colours::white);
            auto checkBounds = toggleBounds.reduced(4);
            g.drawLine(checkBounds.getX() + 2, checkBounds.getCentreY(),
                      checkBounds.getCentreX(), checkBounds.getBottom() - 2, 2.0f);
            g.drawLine(checkBounds.getCentreX(), checkBounds.getBottom() - 2,
                      checkBounds.getRight() - 2, checkBounds.getY() + 2, 2.0f);
        }
        
        g.setColour(juce::Colour(0xff1d1d1f));
        g.setFont(juce::FontOptions(14.0f, juce::Font::bold));
        g.drawText(button.getButtonText(), bounds.removeFromRight(bounds.getWidth() - toggleBounds.getWidth() - 5),
                  juce::Justification::centredLeft);
    }
    
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                     int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override
    {
        auto bounds = juce::Rectangle<float>(0, 0, width, height);
        
        juce::ColourGradient gradient(juce::Colours::white, 0, 0,
                                     juce::Colour(0xfff8f8f8), 0, height, false);
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(bounds, 6.0f);
        
        g.setColour(juce::Colour(0xff007aff));
        g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
        
        auto arrowBounds = juce::Rectangle<float>(buttonX, buttonY, buttonW, buttonH);
        g.setColour(juce::Colour(0xff007aff));
        
        juce::Path arrow;
        arrow.addTriangle(arrowBounds.getCentreX() - 4, arrowBounds.getCentreY() - 2,
                         arrowBounds.getCentreX() + 4, arrowBounds.getCentreY() - 2,
                         arrowBounds.getCentreX(), arrowBounds.getCentreY() + 3);
        g.fillPath(arrow);
    }
};
