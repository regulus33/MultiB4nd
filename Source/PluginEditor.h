/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/LookAndFeel.h"
#include "GUI/UtilityComponents.h"
#include "GUI/GlobalControls.h"
#include "GUI/CompressorBandControls.h"
#include "GUI/SpectrumAnalyzer.h"

/*!
@class SimpleMBCompAudioProcessorEditor

@brief The SimpleMBCompAudioProcessorEditor is a JUCE audio processor editor that is responsible for rendering the UI of a multiband compressor audio effect.

The SimpleMBCompAudioProcessorEditor class provides the basic framework code for a JUCE plugin editor. The class contains member variables for the different components that make up the UI of the audio effect, including the GlobalControls component, the CompressorBandControls component, and the SpectrumAnalyzer component.

The GlobalControls component displays the global controls for the audio effect, such as the attack, release, and threshold parameters. The CompressorBandControls component displays the controls for each individual band of the multiband compressor, such as the gain, ratio, and knee parameters. The SpectrumAnalyzer component displays a visual representation of the audio signal, allowing the user to see the frequency content of the input signal.

The SimpleMBCompAudioProcessorEditor class also includes a member variable for a LookAndFeel object, which is used to provide a custom appearance for the different components in the UI.

The SimpleMBCompAudioProcessorEditor class overrides the paint() and resized() methods to update the appearance of the components in the UI whenever the window is redrawn or resized. The class also has a reference to the SimpleMBCompAudioProcessor object, allowing it to access the processor's parameters and audio data.

@see SimpleMBCompAudioProcessor, GlobalControls, CompressorBandControls, SpectrumAnalyzer
*/
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
    
    Placeholder controlBar;
    // These {} are like little miniature blocks. They are passing in the arg to this
    // class constructor. The objects we reference here must be member variables of this class (at least in this case... maybe there are some other available scope as well)
    GlobalControls globalControls { audioProcessor.apvts };
    // TODO add a breakpoint in this block to see when exactly this block runs
    CompressorBandControls bandControls { audioProcessor.apvts };
    SpectrumAnalyzer analyzer { audioProcessor };
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessorEditor)
};
