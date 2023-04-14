/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "UtilityComponents.h"
#include "GlobalControls.h"
#include "CompressorBandControls.h"



class SimpleMBCompAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleMBCompAudioProcessorEditor (SimpleMBCompAudioProcessor&);
    ~SimpleMBCompAudioProcessorEditor() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
private:
    LookAndFeel lnf;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleMBCompAudioProcessor& audioProcessor;
    
    Placeholder controlBar, analyzer;
    // These {} are like little miniature blocks. They are passing in the arg to this
    // class constructor. The objects we reference here must be member variables of this class (at least in this case... maybe there are some other available scope as well)
    GlobalControls globalControls { audioProcessor.apvts };
    // TODO add a breakpoint in this block to see when exactly this block runs
    CompressorBandControls bandControls { audioProcessor.apvts };
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessorEditor)
};
