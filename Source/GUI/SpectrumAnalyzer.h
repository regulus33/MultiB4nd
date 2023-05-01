/*
 ==============================================================================
 
 SpectrumAnalyzer.h
 Created: 17 Apr 2023 6:15:34pm
 Author:  zack
 
 ==============================================================================
 */

#pragma once
#include <JuceHeader.h>
#include "PathProducer.h"

/*!
 @class SpectrumAnalyzer
 @brief A JUCE component that displays the audio spectrum analysis.
 The SpectrumAnalyzer class is a component that listens to audio processing parameters and displays the audio spectrum analysis. It uses a PathProducer to generate paths from the audio data, and draws these paths on the component. The component also handles redrawing the background grid and text labels as needed.
 */
struct SpectrumAnalyzer: juce::Component,
juce::AudioProcessorParameter::Listener,
juce::Timer
{
    SpectrumAnalyzer(SimpleMBCompAudioProcessor&);
    ~SpectrumAnalyzer();
    
    void parameterValueChanged (int parameterIndex, float newValue) override;
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override { }
    
    void timerCallback() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void toggleAnalysisEnablement(bool enabled)
    {
        shouldShowFFTAnalysis = enabled;
    }
private:
    SimpleMBCompAudioProcessor& audioProcessor;
    
    bool shouldShowFFTAnalysis = true;
    
    juce::Atomic<bool> parametersChanged { false };
    
//    void drawBackgroundGrid(juce::Graphics& g);
    void drawBackgroundGrid(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawTextLabels(juce::Graphics& g, juce::Rectangle<int> bounds);
    
    std::vector<float> getFrequencies();
    std::vector<float> getGains();
    std::vector<float> getXs(const std::vector<float>& freqs, float left, float width);
    
    juce::Rectangle<int> getRenderArea(juce::Rectangle<int> bounds);
    
    juce::Rectangle<int> getAnalysisArea(juce::Rectangle<int> bounds);
    
    PathProducer leftPathProducer, rightPathProducer;
    
    void drawFFTAnalysis(juce::Graphics& g, juce::Rectangle<int> bounds);
};
