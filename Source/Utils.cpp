/*
  ==============================================================================

    Utils.cpp
    Created: 14 Apr 2023 11:49:32pm
    Author:  zack

  ==============================================================================
*/

#include "Utils.h"

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

void drawModuleBackground(juce::Graphics &g, juce::Rectangle<int> bounds)
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
}

