/*
 ==============================================================================
 
 UtilityComponents.h
 Created: 14 Apr 2023 7:34:50pm
 Author:  zack
 
 
 ==============================================================================
 */

#pragma once
#include <JuceHeader.h>


/*!
 @class Placeholder
 @brief Placeholder component for UI design purposes.
 
 This class represents a placeholder component that can be used in UI design to temporarily occupy a space until the final component is added. The placeholder component is a simple rectangle with a specified color. The color can be set using the customColor member variable. The component is painted using the paint() method, which draws the rectangle with the specified color.
 */
struct Placeholder : juce::Component
{
    Placeholder();
    
    void paint(juce::Graphics& g) override;
    juce::Colour customColor;
};


/*!
 @class RotarySlider
 @brief A rotary slider component for UI design purposes.
 
 This class represents a rotary slider component that can be used in UI design. It is a subclass of the juce::Slider class and inherits all its functionality. The appearance of the rotary slider is customized to look like a rotary knob.
 */
struct RotarySlider : public juce::Slider
{
    RotarySlider();
};
