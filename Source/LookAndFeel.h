/*
  ==============================================================================

    LookAndFeel.h
    Created: 14 Apr 2023 7:10:37pm
    Author:  zack

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>


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
