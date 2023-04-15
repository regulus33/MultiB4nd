/*
  ==============================================================================

    UtilityComponents.cpp
    Created: 14 Apr 2023 7:34:50pm
    Author:  zack

  ==============================================================================
*/

#include "UtilityComponents.h"

Placeholder::Placeholder()
{
    juce::Random r;
    customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}

void Placeholder::paint(juce::Graphics& g)
{
    g.fillAll(customColor);
}

RotarySlider::RotarySlider() :juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                           juce::Slider::TextEntryBoxPosition::NoTextBox)
{}
