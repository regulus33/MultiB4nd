/*
 ==============================================================================
 
 CompressorBand.h
 Created: 14 Apr 2023 7:58:06pm
 Author:  zack
 
 ==============================================================================
 */

#pragma once
#include <JuceHeader.h>
#include "../GUI/Utils.h"


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
    
    float getRmsLevelInputDb() const { return rmsLevelInputDb; }
    float getRmsLevelOutputDb() const { return rmsLevelOutputDb; }
private:
    juce::dsp::Compressor<float> compressor;
    
    std::atomic<float> rmsLevelInputDb { NEGATIVE_INFINITY };
    std::atomic<float> rmsLevelOutputDb { NEGATIVE_INFINITY };
    
    /*!
     @brief computes the RMS or "average energy / loudness calc thingy" of the buffer.
     */
    template<typename T>
    float computeRMSLevel(const T& buffer)
    {
        // get num channels and samples
        // compute rms of each channel and add all together
        // divide by num channels
        int numChannels = static_cast<int>(buffer.getNumChannels());
        int numSamples = static_cast<int>(buffer.getNumSamples());
        auto rms = 0.f;
        for(int chan = 0; chan < numChannels; ++chan)
        {
            rms += buffer.getRMSLevel(chan, 0, numSamples);
        }
        
        rms /= static_cast<float>(numChannels);
        return rms;
    }
};
