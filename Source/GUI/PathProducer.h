/*
  ==============================================================================

    PathProducer.h
    Created: 17 Apr 2023 6:15:19pm
    Author:  zack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "FFTDataGenerator.h"
#include "AnalyzerPathGenerator.h"
#include "../PluginProcessor.h"

/**

@struct PathProducer
@brief A class that processes FFT data and generates paths.
PathProducer class processes FFT data and generates paths that can be used to render audio data. It uses a LeftChannelFifo buffer to store incoming audio data, a monoBuffer to store the mono representation of the audio data, and LeftChannelFFTDataGenerator to generate FFT data.
*/
struct PathProducer
{
    PathProducer(SingleChannelSampleFifo<SimpleMBCompAudioProcessor::BlockType>& scsf) :
            leftChannelFifo(&scsf)
    {
        leftChannelFFTDataGenerator.changeOrder(FFTOrder::order2048);
        monoBuffer.setSize(1, leftChannelFFTDataGenerator.getFFTSize());
    }
    void process(juce::Rectangle<float> fftBounds, double sampleRate);
    juce::Path getPath() { return leftChannelFFTPath; }
    
    void updateNegativeInfinity(float nf) { negativeInfinity = nf; }
private:
    SingleChannelSampleFifo<SimpleMBCompAudioProcessor::BlockType>* leftChannelFifo;

    juce::AudioBuffer<float> monoBuffer;

    FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator;

    AnalyzerPathGenerator<juce::Path> pathProducer;

    juce::Path leftChannelFFTPath;
    
    float negativeInfinity {-48.f};
};
