/*
 ==============================================================================
 
 GlobalControls.h
 Created: 14 Apr 2023 7:54:06pm
 Author:  zack
 
 ==============================================================================
 */

#pragma once
#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"

/*!
 @class GlobalControls
 @brief A JUCE component that displays global control sliders for audio processing.
 This component displays the four global control sliders: inGain, lowMidXover, midHighXover, and outGain. It utilizes the RotarySliderWithLabels class to create each of these sliders.
 */
struct GlobalControls : juce::Component
{
    GlobalControls(juce::AudioProcessorValueTreeState& apvts);
    
    void paint(juce::Graphics& g) override;
    
    void resized() override;
private:
    using RSWL = RotarySliderWithLabels;
    std::unique_ptr<RSWL> inGainSlider, lowMidXoverSlider, midHighXoverSlider, outGainSlider;
    
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    std::unique_ptr<Attachment> lowMidXoverSliderAttachment,
    midHighXoverSliderAttachment,
    inGainSliderAttachment,
    outGainSliderAttachment;
};
