/*
  ==============================================================================

    CompressorBand.cpp
    Created: 14 Apr 2023 7:58:06pm
    Author:  zack

  ==============================================================================
*/

#include "CompressorBand.h"

void CompressorBand::prepare(const juce::dsp::ProcessSpec& spec)
{
    compressor.prepare(spec);
}

void CompressorBand::updateCompressorSettings()
{
    compressor.setAttack(attack->get());
    compressor.setRelease(release->get());
    compressor.setThreshold(threshold->get());
    compressor.setRatio(
                        ratio->getCurrentChoiceName().getFloatValue());
}

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
