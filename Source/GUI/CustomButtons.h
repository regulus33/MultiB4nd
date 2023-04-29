/*
 ==============================================================================
 
 CustomButtons.h
 Created: 14 Apr 2023 7:31:05pm
 Author:  zack
 
 ==============================================================================
 */

#pragma once
#include <JuceHeader.h>

/*!
 @struct PowerButton
 @brief A custom implementation of the JUCE ToggleButton.
 This struct provides a custom implementation of the JUCE ToggleButton, which allows the user to turn on and off the power.
*/
struct PowerButton : juce::ToggleButton { };

/*!
 @struct AnalyzerButton
 @brief A custom implementation of the JUCE ToggleButton for the analyzer.
 This struct provides a custom implementation of the JUCE ToggleButton for the analyzer, which allows the user to turn on and off the analysis.
 */
struct AnalyzerButton : juce::ToggleButton
{
    
    void resized() override;
    /*!
     @var juce::Path AnalyzerButton::randomPath
     @brief The path used for the analyzer.
     This path is used to display the analyzer information in the user interface.
     */
    juce::Path randomPath;
};
