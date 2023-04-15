/*
  ==============================================================================

    CustomButtons.h
    Created: 14 Apr 2023 7:31:05pm
    Author:  zack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct PowerButton : juce::ToggleButton { };

struct AnalyzerButton : juce::ToggleButton
{
    void resized() override;
    juce::Path randomPath;
};
