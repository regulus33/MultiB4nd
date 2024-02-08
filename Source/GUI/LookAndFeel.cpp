/*
 ==============================================================================
 
 LookAndFeel.cpp
 Created: 14 Apr 2023 7:10:37pm
 Author:  zack
 
 ==============================================================================
 */

#include "LookAndFeel.h"
#include "RotarySliderWithLabels.h"
#include "UtilityComponents.h"
#include "CustomButtons.h"

/*!
 allows you to pick the color of a given element.
 NOTE you need to include ColorScheme to use this
 NOTE there will always be one jassert, just skip past it
 */
#define USE_LIVE_CONSTANT true
#if USE_LIVE_CONSTANT
#define colorHelper(c) JUCE_LIVE_CONSTANT(c);
#else
#define colorHelper(c) c;
#endif

namespace ColorScheme
{
inline juce::Colour getSliderBorderColor()
{
    return colorHelper(juce::Colours::white);
}
inline juce::Colour getSliderFillColor()
{
    return colorHelper(juce::Colours::black);
}
}

/*!
 * @brief Draws the rotary slider component.
 *
 * This function is called by JUCE when it needs to draw the rotary slider component.
 * It draws the rotary slider component with a custom appearance based on the input parameters.
 *
 * @param g The graphics context to draw on.
 * @param x The x-coordinate of the component.
 * @param y The y-coordinate of the component.
 * @param width The width of the component.
 * @param height The height of the component.
 * @param sliderPosProportional The position of the slider expressed as a value between 0.0 and 1.0.
 * @param rotaryStartAngle The start angle of the rotary slider in radians.
 * @param rotaryEndAngle The end angle of the rotary slider in radians.
 * @param slider The slider component to be drawn.
 */
void LookAndFeel::drawRotarySlider(juce::Graphics & g,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider & slider)
{
    using namespace juce;
    /* create the boundary of the slider's area (its a rectangular zone) */
    auto bounds = Rectangle<float>(x, y, width, height);
    
    auto enabled = slider.isEnabled();
    
    /* set the fill color of the circle */
    g.setColour(enabled ?  ColorScheme::getSliderFillColor() : Colours::grey);
    /* fit a circle whose diameter is exactly the width of the rectangular bounds */
    g.fillEllipse(bounds);
    
    g.setColour(enabled ? ColorScheme::getSliderBorderColor() : Colours::grey);
    g.drawEllipse(bounds, 1.f);
    
    if( auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        /*! get the center of the circle */
        auto center = bounds.getCentre();
        Path p;
        
        Rectangle<float> notch;
        notch.setLeft(center.getX() - 2);
        notch.setRight(center.getX() + 2);
        notch.setTop(bounds.getY());
        notch.setBottom(center.getY() - rswl->getTextHeight() * 1.5);
        
        p.addRoundedRectangle(notch, 2.f);
        
        jassert(rotaryStartAngle < rotaryEndAngle);
        
        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
        
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        
        g.fillPath(p);
        
        g.setFont(rswl->getTextHeight());
        auto text = rswl->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);
        
        notch.setSize(strWidth + 4, rswl->getTextHeight() + 2);
        notch.setCentre(bounds.getCentre());
        
        g.setColour(enabled ? Colours::black : Colours::darkgrey);
        g.fillRect(notch);
        
        g.setColour(enabled ? Colours::white : Colours::lightgrey);
        g.drawFittedText(text, notch.toNearestInt(), juce::Justification::centred, 1);
    }
}

/*!
 * @brief Draws the toggle button component.
 *
 * This function is called by JUCE when it needs to draw the toggle button component.
 * It draws the toggle button component with a custom appearance based on the input parameters.
 *
 * @param g The graphics context to draw on.
 * @param toggleButton The toggle button component to be drawn.
 * @param shouldDrawButtonAsHighlighted Indicates whether the button should be drawn as highlighted.
 * @param shouldDrawButtonAsDown Indicates whether the button should be drawn as pressed.
 */
void LookAndFeel::drawToggleButton(juce::Graphics &g,
                                   juce::ToggleButton &toggleButton,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown)
{
    using namespace juce;
    
    if( auto* pb = dynamic_cast<PowerButton*>(&toggleButton) )
    {
        Path powerButton;
        
        auto bounds = toggleButton.getLocalBounds();
        
        auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 6;
        auto r = bounds.withSizeKeepingCentre(size, size).toFloat();
        
        float ang = 30.f; //30.f;
        
        size -= 6;
        
        powerButton.addCentredArc(r.getCentreX(),
                                  r.getCentreY(),
                                  size * 0.5,
                                  size * 0.5,
                                  0.f,
                                  degreesToRadians(ang),
                                  degreesToRadians(360.f - ang),
                                  true);
        
        powerButton.startNewSubPath(r.getCentreX(), r.getY());
        powerButton.lineTo(r.getCentre());
        
        PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);
        
        auto color = toggleButton.getToggleState() ? Colours::dimgrey : Colour(0u, 172u, 1u);
        
        g.setColour(color);
        g.strokePath(powerButton, pst);
        g.drawEllipse(r, 2);
    }
    else if( auto* analyzerButton = dynamic_cast<AnalyzerButton*>(&toggleButton) )
    {
        auto color = ! toggleButton.getToggleState() ? Colours::dimgrey : Colour(0u, 172u, 1u);
        
        g.setColour(color);
        
        auto bounds = toggleButton.getLocalBounds();
        g.drawRect(bounds);
        
        g.strokePath(analyzerButton->randomPath, PathStrokeType(1.f));
    }
    else
    {
        auto bounds = toggleButton.getLocalBounds().reduced(2);
        auto buttonIsOn = toggleButton.getToggleState();
        const int cornerSize = 4;
        
        //NOTE: g.setColour() function sets the color that is used for any subsequent drawing operations until a new color is set using g.setColour() with a different color.
        g.setColour(buttonIsOn ?
                    toggleButton.findColour(TextButton::ColourIds::buttonOnColourId) :
                    toggleButton.findColour(TextButton::ColourIds::buttonColourId));
        
        g.fillRoundedRectangle(bounds.toFloat(), cornerSize);
        
        g.setColour(buttonIsOn ? juce::Colours::black : juce::Colours::white);
        g.drawRoundedRectangle(bounds.toFloat(), cornerSize, 1);
        g.drawFittedText(toggleButton.getName(), bounds, juce::Justification::centred, 1);
    }
}
