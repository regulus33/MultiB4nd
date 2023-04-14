/*
  ==============================================================================

    UtilityComponents.h
    Created: 14 Apr 2023 7:34:50pm
    Author:  zack
 

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct Placeholder : juce::Component
{
    Placeholder();
    
    void paint(juce::Graphics& g) override;
    juce::Colour customColor;
};

struct RotarySlider : public juce::Slider
{
    RotarySlider();
};
