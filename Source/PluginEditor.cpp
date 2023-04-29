/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GUI/LookAndFeel.h"
#include "GUI/UtilityComponents.h"
#include "GUI/GlobalControls.h"
#include "GUI/CompressorBandControls.h"
#include "DSP/Params.h"

//==============================================================================

/*!
 @brief Top level rendering function of the UI, this is where all the main components are rendered. LookAndFeel is also set.
 */
SimpleMBCompAudioProcessorEditor::SimpleMBCompAudioProcessorEditor (SimpleMBCompAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&lnf);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //    addAndMakeVisible(controlBar);
    addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);
    
    setSize (600, 500);
}

/*!
 @brief Destructor. It unsets LookAndFeel.
 */
SimpleMBCompAudioProcessorEditor::~SimpleMBCompAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

/*!
 @brief Sets font color, etc. for the main parent UI component.
 */
void SimpleMBCompAudioProcessorEditor::paint (juce::Graphics& g)
{
    //    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    //
    //    g.setColour (juce::Colours::white);
    //    g.setFont (15.0f);
    //    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    
    g.fillAll(juce::Colours::black);
}

/*!
 @begin Sets boundaries for the child components and adds some padding.
 */
void SimpleMBCompAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    //top section
    controlBar.setBounds( bounds.removeFromTop(32) );
    
    //bottom section
    bandControls.setBounds( bounds.removeFromBottom(135) );
    
    // middle section
    analyzer.setBounds(bounds.removeFromTop(225));
    
    // second from bottom. remainder of bounds
    globalControls.setBounds(bounds);
    
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
