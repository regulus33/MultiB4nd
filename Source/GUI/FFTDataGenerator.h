/*
 ==============================================================================
 
 FFTDataGenerator.h
 Created: 17 Apr 2023 6:04:43pm
 Author:  zack
 
 ==============================================================================
 */

#pragma once
#include <JuceHeader.h>
#include "Utils.h"
#include "../DSP/Fifo.h"

/*!
 @brief This struct performs an FFT transformation on audio data, applies a windowing function, normalizes the FFT values, and converts the values to decibels. The resulting FFT data is stored in a FIFO buffer. The order of the FFT calculation can also be changed.
 
 Lets look at a simple JS audio visualizer to understand FFT visualization
 There will be some FFTGenerator class that produces an array. Lets call it frequencyData.
 
 @code
 const frequencyData = new Uint8Array(analyserNode.frequencyBinCount);
 analyserNode.getByteFrequencyData(frequencyData);
 @endcode
 
 Each value in the frequencyData array represents the amplitude of a particular frequency band in the audio signal at brief moment in time. The AnalyserNode.frequencyBinCount property specifies the number of frequency bands used for analysis, and the frequencyData array has a length equal to AnalyserNode.frequencyBinCount.
 
 @code
 [ 23, 45, 68, 90, 112, 134, 155, 177, 198, 220, 241, 255, 250, 236, 223, 210, 197, 184]`
 [ 33, 55, 78, 100, 122, 144, 165, 187, 208, 230, 251, 265, 260, 246, 233, 220, 207, 194]`
 [ 53, 75, 98, 120, 142, 164, 185, 207, 228, 250, 271, 285, 280, 266, 253, 240, 227, 214]`
 [ 83, 105, 128, 150, 172, 194, 215, 237, 258, 280, 301, 315, 310, 296, 283, 270, 257, 244]`
 [ 123, 145, 168, 190, 212, 234, 255, 277, 298, 320, 341, 355, 350, 336, 323, 310, 297, 284]`
 @endcode
 
 The array is the size of exactly the amount of *BANDS* that FFT determines are existing in the current signal source.
 Then frame by frame we iterate through our array
 
 @code
 for(let band = 0;  band < frequencyData.length; band++)
 {
 drawBarWithHeight(band);
 }
 @endcode
 
 Now look at graph below. X axis is index of array (band) and Y axis is the amplitude of that band:
@code
 AUDIO ANALYSIS OF FFT ARRAY WITH 58 bands
 
 |          |   ||    |  |  |       |  |    |    |
 ||         |  |||    |  |  |       | ||  | |    | |
 |||| | |   || |||| | || || | |     | ||| | | | || |    |  |
 |||| | || |||||||| |||| ||||||  | || ||||| | | ||||    | ||
 |||| |||| ||||||||||||||||||||| |||| ||||||||| ||||| | | ||
 ||||||||||||||||||||||||||||||| |||||||||||||||||||| | ||||
 |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 |0        |10       |20       |30       |40       |50
 @endcode
 */
template<typename BlockType>
struct FFTDataGenerator
{
    /**
     @brief This function produces FFT data for rendering purposes.
     The function takes in audio data as input and performs FFT transformation on it, after applying a windowing function and normalizing the FFT values. The values are then converted to decibels. The final FFT data is stored in the fftDataFifo member variable.
     @param audioData The input audio data on which FFT transformation is performed.
     @param negativeInfinity The negative infinity value used for converting the values to decibels.
     */
    void produceFFTDataForRendering(const juce::AudioBuffer<float>& audioData, const float negativeInfinity)
    {
        const auto fftSize = getFFTSize();
        
        fftData.assign(fftData.size(), 0);
        auto* readIndex = audioData.getReadPointer(0);
        std::copy(readIndex, readIndex + fftSize, fftData.begin());
        
        /*! first apply a windowing function to our data */
        window->multiplyWithWindowingTable (fftData.data(), fftSize);       // [1]
        
        /*! render the data */
        forwardFFT->performFrequencyOnlyForwardTransform (fftData.data());  // [2]
        
        int numBins = (int)fftSize / 2;
        
        //normalize the fft values.
        for( int i = 0; i < numBins; ++i )
        {
            auto v = fftData[i];
            //            fftData[i] /= (float) numBins;
            if( !std::isinf(v) && !std::isnan(v) )
            {
                v /= float(numBins);
            }
            else
            {
                v = 0.f;
            }
            fftData[i] = v;
        }
        float max = negativeInfinity;
        
        //convert them to decibels
        for( int i = 0; i < numBins; ++i )
        {
            auto data =  juce::Decibels::gainToDecibels(fftData[i], negativeInfinity);
            fftData[i] = data;
            max = juce::jmax(data, max);
        }
        
//       jassertfalse;
        
        fftDataFifo.push(fftData);
    }
    
    
    /*!
     @brief Changes the FFT order to the specified value
     This function changes the order of the FFT calculation and updates the relevant objects accordingly.
     The new FFT order is specified by the newOrder parameter.
     The function also recreates the windowing function, forward FFT object, FFT data buffer, and the FFT data FIFO buffer.
     Additionally, the FIFO index is reset.
     @param newOrder New order of the FFT calculation example 2048
     */
    void changeOrder(FFTOrder newOrder)
    {
        //when you change order, recreate the window, forwardFFT, fifo, fftData
        //also reset the fifoIndex
        //things that need recreating should be created on the heap via std::make_unique<>
        
        order = newOrder;
        auto fftSize = getFFTSize();
        
        forwardFFT = std::make_unique<juce::dsp::FFT>(order);
        window = std::make_unique<juce::dsp::WindowingFunction<float>>(fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris);
        
        fftData.clear();
        fftData.resize(fftSize * 2, 0);
        
        fftDataFifo.prepare(fftData.size());
    }
    //==============================================================================
    int getFFTSize() const { return 1 << order; }
    int getNumAvailableFFTDataBlocks() const { return fftDataFifo.getNumAvailableForReading(); }
    //==============================================================================
    bool getFFTData(BlockType& fftData) { return fftDataFifo.pull(fftData); }
private:
    FFTOrder order;
    BlockType fftData;
    std::unique_ptr<juce::dsp::FFT> forwardFFT;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;
    
    Fifo<BlockType> fftDataFifo;
};
