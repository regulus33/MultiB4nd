/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
Placeholder::Placeholder()
{
    juce::Random r;
    customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}


void GlobalControls::paint(juce::Graphics &g)
{
    using namespace juce;
    auto bounds = getLocalBounds();
    // make whole rectangle violet
    g.setColour(Colours::blueviolet);
    g.fillAll();
    
    auto localBounds = bounds;
    
    // *This is like a way to make a border
    //  1. you calculate a smaller rectangle within the larger rectangle
    bounds.reduce(3,3);
    g.setColour(Colours::black);
    // 2. you fill the part of the rectangle what fits within your reduced bounds. Original fill remains as a border.
    g.fillRoundedRectangle(bounds.toFloat(), 3);
    
    g.drawRect(localBounds);
}

//==============================================================================
SimpleMBCompAudioProcessorEditor::SimpleMBCompAudioProcessorEditor (SimpleMBCompAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
//    addAndMakeVisible(controlBar);
//    addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);
//    addAndMakeVisible(bandControls);
    
    setSize (600, 500);
}

SimpleMBCompAudioProcessorEditor::~SimpleMBCompAudioProcessorEditor()
{
}

//==============================================================================
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
