/*
 ==============================================================================
 
 AnalyzerPathGenerator.h
 Created: 17 Apr 2023 6:15:08pm
 Author:  zack
 
 ==============================================================================
 */

#pragma once
#include <JuceHeader.h>
#include "../DSP/Fifo.h"

/*!
 @brief A helper class that generates a juce::Path from an array of float data
 This class uses a Fifo to store the generated paths. The user can get the number of paths available and retrieve a path.
 */
 template<typename PathType>
 struct AnalyzerPathGenerator
{
    /*!
     Converts the renderData array into a juce::Path
     @param renderData The array of float data to be converted into a path
     @param fftBounds The bounds of the FFT
     @param fftSize The size of the FFT
     @param binWidth The width of each bin
     @param negativeInfinity The negative infinity value
     */
    void generatePath(const std::vector<float>& renderData,
                      juce::Rectangle<float> fftBounds,
                      int fftSize,
                      float binWidth,
                      float negativeInfinity)
    {
        auto top = fftBounds.getY();
        auto bottom = fftBounds.getBottom();
        //        auto bottom = fftBounds.getHeight();
        auto width = fftBounds.getWidth();
        
        int numBins = (int)fftSize / 2;
        
        PathType p;
        p.preallocateSpace(3 * (int)fftBounds.getWidth());
        
        auto map = [bottom, top, negativeInfinity](float v)
        {
            return juce::jmap(v,
                              negativeInfinity, 0.f,
                              //                            float(bottom+10),
                              bottom,
                              top);
            
        };
        
        auto y = map(renderData[0]);
        
        //        jassert( !std::isnan(y) && !std::isinf(y) );
        if( std::isnan(y) || std::isinf(y) )
            y = bottom;
        
        p.startNewSubPath(0, y);
        
        const int pathResolution = 2; //you can draw line-to's every 'pathResolution' pixels.
        
        for( int binNum = 1; binNum < numBins; binNum += pathResolution )
        {
            y = map(renderData[binNum]);
            
            //            jassert( !std::isnan(y) && !std::isinf(y) );
            
            if( !std::isnan(y) && !std::isinf(y) )
            {
                auto binFreq = binNum * binWidth;
                auto normalizedBinX = juce::mapFromLog10(binFreq, 20.f, 20000.f);
                int binX = std::floor(normalizedBinX * width);
                p.lineTo(binX, y);
            }
        }
        
        pathFifo.push(p);
    }
    
    int getNumPathsAvailable() const
    {
        return pathFifo.getNumAvailableForReading();
    }
    
    bool getPath(PathType& path)
    {
        return pathFifo.pull(path);
    }
private:
    Fifo<PathType> pathFifo;
};
