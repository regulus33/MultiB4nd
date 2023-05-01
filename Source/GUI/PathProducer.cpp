/*
 ==============================================================================
 
 PathProducer.cpp
 Created: 17 Apr 2023 6:15:19pm
 Author:  zack
 
 ==============================================================================
 */

#include "PathProducer.h"

/*!
 @brief Processes FFT data and generates paths.
 This function processes FFT data and generates paths that can be used to render audio data. It uses a LeftChannelFifo buffer to store incoming audio data, a monoBuffer to store the mono representation of the audio data, and LeftChannelFFTDataGenerator to generate FFT data.
 @param fftBounds The bounds of the FFT data to be rendered.
 @param sampleRate The sample rate of the audio data.
 */
void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate)
{
    juce::AudioBuffer<float> tempIncomingBuffer;
    while( leftChannelFifo->getNumCompleteBuffersAvailable() > 0 )
    {
        if( leftChannelFifo->getAudioBuffer(tempIncomingBuffer) )
        {
            auto size = tempIncomingBuffer.getNumSamples();
            
            // s
            jassert(size <= monoBuffer.getNumSamples());
            size = juce::jmin(size, monoBuffer.getNumSamples());
            
            auto writePointer = monoBuffer.getWritePointer(0, 0);
            auto readPointer = monoBuffer.getReadPointer(0, size);
            std::copy(readPointer, (readPointer + monoBuffer.getNumSamples() - size), writePointer);
            
//            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, 0),
//                                              monoBuffer.getReadPointer(0, size),
//                                              monoBuffer.getNumSamples() - size);
            
            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),
                                              tempIncomingBuffer.getReadPointer(0, 0),
                                              size);
            
            leftChannelFFTDataGenerator.produceFFTDataForRendering(monoBuffer, negativeInfinity);
        }
    }
    
    const auto fftSize = leftChannelFFTDataGenerator.getFFTSize();
    const auto binWidth = sampleRate / double(fftSize);
    
    while( leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0 )
    {
        std::vector<float> fftData;
        if( leftChannelFFTDataGenerator.getFFTData( fftData) )
        {
            pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, negativeInfinity);
        }
    }
    
    while( pathProducer.getNumPathsAvailable() > 0 )
    {
        pathProducer.getPath( leftChannelFFTPath );
    }
}

