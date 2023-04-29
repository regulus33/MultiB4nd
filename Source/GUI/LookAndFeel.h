/*
 ==============================================================================
 
 LookAndFeel.h
 Created: 14 Apr 2023 7:10:37pm
 Author:  zack
 
 ==============================================================================
 */
#pragma once
#include <JuceHeader.h>

/*!
 @struct LookAndFeel
 @brief A struct that extends the juce::LookAndFeel_V4 class.
 This struct provides custom implementations of the drawRotarySlider() and drawToggleButton() methods,
 which are used to display rotary sliders and toggle buttons in the graphical user interface.
 */
struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics&,
                           int x, int y, int widt1h, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider&) override;
    
    void drawToggleButton (juce::Graphics &g,
                           juce::ToggleButton & toggleButton,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;
};
