/*
 ==============================================================================
 
 CompressorBand.h
 Created: 14 Apr 2023 7:58:06pm
 Author:  zack
 
 ==============================================================================
 */

#pragma once
#include <JuceHeader.h>


/*!
 @class CompressorBand
 @brief CompressorBand class encapsulates all the audio parameters related to a band compressor and implements audio processing.
 This class holds the audio parameters such as attack, release, threshold, ratio, bypassed, mute, and solo. The prepare method sets up the compressor with the given process specification. The updateCompressorSettings method updates the parameters of the compressor. The process method processes the audio buffer.
 */
struct CompressorBand
{
    juce::AudioParameterFloat* attack { nullptr };
    juce::AudioParameterFloat* release { nullptr };
    juce::AudioParameterFloat* threshold { nullptr };
    juce::AudioParameterChoice* ratio { nullptr };
    juce::AudioParameterBool* bypassed { nullptr };
    juce::AudioParameterBool* mute { nullptr };
    juce::AudioParameterBool* solo { nullptr };
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void updateCompressorSettings();
    void process(juce::AudioBuffer<float>& buffer);
private:
    juce::dsp::Compressor<float> compressor;
};
