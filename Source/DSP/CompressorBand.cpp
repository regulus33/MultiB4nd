/*
  ==============================================================================

    CompressorBand.cpp
    Created: 14 Apr 2023 7:58:06pm
    Author:  zack

  ==============================================================================
*/

#include "CompressorBand.h"

/*!
@brief Prepares the compressor for processing by setting up its internal processing data based on the given audio processing specification
@param spec The audio processing specification which contains the sample rate, buffer size and number of channels
*/
void CompressorBand::prepare(const juce::dsp::ProcessSpec& spec)
{
    compressor.prepare(spec);
}

/*!
@brief Updates the compressor settings by setting the attack, release, threshold and ratio values from their corresponding audio parameters
*/
void CompressorBand::updateCompressorSettings()
{
    compressor.setAttack(attack->get());
    compressor.setRelease(release->get());
    compressor.setThreshold(threshold->get());
    compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
}

/*!
 @brief Processes the audio buffer by either bypassing the processing or by applying the compression based on the bypass status
 @param buffer The audio buffer to be processed
*/
void CompressorBand::process(juce::AudioBuffer<float>& buffer)
{
    // Create our own block to manipulate. Blocks are basically the narrowed down version of the buffer.
    // focused -> channels, range etc.
    auto block = juce::dsp::AudioBlock<float>(buffer);
    
    // context -> sample-rate buffer-size etc.
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    
    // Bypass the whole processBlock code (anything we would do is not done)
    context.isBypassed = bypassed->get();
    
    // We are just passing our context pointer to compressor overwriting it and another process will read from the same buffer to the output
    compressor.process(context);
}
