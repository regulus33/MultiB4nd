/*
 ==============================================================================
 
 Utils.cpp
 Created: 14 Apr 2023 11:49:32pm
 Author:  zack
 
 ==============================================================================
 */

#include "Utils.h"

/*!
 @brief Generates a user friendly string based on the audio parameter type it represents and the value itself of the parameter.
 @param param The ranged audio parameter for which the value string is to be returned.
 @param getLow Indicates whether the low value or high value of the parameter range should be used.
 @param suffix The suffix to be added to the string representation of the value.
 @return juce::String The string representation of the value.
 */
juce::String getValString(const juce::RangedAudioParameter& param, bool getLow, juce::String suffix)
{
    juce::String str;
    
    auto val = getLow ? param.getNormalisableRange().start : param.getNormalisableRange().end;
    
    bool useK = truncateKiloValue(val);
    str << val;
    
    if( useK )
        str << "k";
    
    str << suffix;
    
    return str;
}

/*!
 This function draws a rectangle with a border using the JUCE Graphics object.
 @param g Graphics object belonging to whatever component you are drawing background of.
 @param bounds The bounds of whatever component you are drawing background of.
 The function first sets the color of the Graphics object to Colours::blueviolet and fills the entire rectangle.
 Then it calculates a smaller rectangle within the larger rectangle by reducing the bounds by 3 pixels on each side.
 The color of the Graphics object is then set to Colours::black and the smaller rectangle is filled with rounded corners.
 Finally, the original rectangle is drawn. The result is a rectangle with a border.
 @return juce::Rectangle<int> representing the bounds of the background.
 */
juce::Rectangle<int> drawModuleBackground(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    using namespace juce;
    // make whole rectangle violet
    g.setColour(Colours::blueviolet);
    g.fillAll();
    
    auto localBounds = bounds;
    
    // *This is like a way to make a border
    //  1. you calculate a smaller rectangle within the larger rectangle
    bounds.reduce(3,3);
    g.setColour(Colours::black);
    // 2. you fill the part of the rectangle what fits within your reduced bounds. Original fill remains as a border.
    g.fillRoundedRectangle(bounds.toFloat(), 3);
    
    g.drawRect(localBounds);
    
    return bounds;
}

