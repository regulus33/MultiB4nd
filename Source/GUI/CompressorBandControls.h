/*
  ==============================================================================

    CompressorBandControls.h
    Created: 14 Apr 2023 7:50:49pm
    Author:  zack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"

struct CompressorBandControls : juce::Component, juce::Button::Listener
{
    CompressorBandControls(juce::AudioProcessorValueTreeState& apvts);
    ~CompressorBandControls() override;
    void resized() override;
    void paint(juce::Graphics& g) override;
    void buttonClicked(juce::Button* button) override;
private:
    static const juce::TextButton::ColourIds ButtonOnColorId { juce::TextButton::ColourIds::buttonOnColourId };
    static const juce::TextButton::ColourIds ButtonOffColorId { juce::TextButton::ColourIds::buttonColourId };
    juce::AudioProcessorValueTreeState&  apvts;
    
    RotarySliderWithLabels attackSlider, releaseSlider, thresholdSlider;
    RatioSlider ratioSlider;
    
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> attackSliderAttachment,
                                releaseSliderAttachment,
                                thresholdSliderAttachment,
                                ratioSliderAttachment;
    
    juce::ToggleButton bypassButton, soloButton, muteButton, lowBand, midBand, highBand;
    
    using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<BtnAttachment> bypassButtonAttachment,
                                   soloButtonAttachment,
                                    muteButtonAttachment;
    
    juce::Component::SafePointer<CompressorBandControls> safePtr {this};
    
    // NOTE: important
    juce::ToggleButton* activeBand = {&lowBand};
    
    void updateAttachments();
    void updateSliderEnabledMents();
    void updateSoloMuteBypassToggleStates(juce::Button& button);
    void updateActiveBandFillColors(juce::Button& clickedButton);
    void resetActiveBandColors();
    static void refreshBandButtonColors(juce::Button& band, juce::Button& colorSource);
    // TODO: we are making this called in constructor to update band button colors
    // independent of clicking
    // 6:00:55
    void updateBandSelectButtonStates();
};
