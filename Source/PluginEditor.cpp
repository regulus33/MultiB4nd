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

ControlBar::ControlBar()
{
    analyzerButton.setToggleState(true, juce::NotificationType::dontSendNotification);
    addAndMakeVisible(analyzerButton);
    addAndMakeVisible(globalBypassButton);
}

void ControlBar::resized()
{
    auto bounds = getLocalBounds();
    
    analyzerButton.setBounds(bounds.removeFromLeft(50).withTrimmedTop(4).withTrimmedBottom(4));
    globalBypassButton.setBounds(bounds.removeFromRight(50).withTrimmedTop(2).withTrimmedBottom(2));
}

/*!
 @brief Top level rendering function of the UI, this is where all the main components are rendered. LookAndFeel is also set.
 */
SimpleMBCompAudioProcessorEditor::SimpleMBCompAudioProcessorEditor (SimpleMBCompAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&lnf);
    
    controlBar.analyzerButton.onClick = [this]()
    {
        auto shouldBeOn = controlBar.analyzerButton.getToggleState();
        analyzer.toggleAnalysisEnablement(shouldBeOn);
    };
    
    controlBar.globalBypassButton.onClick = [this]()
    {
        toggleGlobalBypassState();
    };
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(controlBar);
    addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);
    
    setSize (600, 500);
    
    startTimerHz(60);
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

void SimpleMBCompAudioProcessorEditor::timerCallback()
{
    // TODO: this is suspicious... make sure its not bad practice to work with a dynamic structure in this callback
    std::vector<float> values
    {
        audioProcessor.lowBandComp.getRmsLevelInputDb(),
        audioProcessor.lowBandComp.getRmsLevelOutputDb(),
        audioProcessor.midBandComp.getRmsLevelInputDb(),
        audioProcessor.midBandComp.getRmsLevelOutputDb(),
        audioProcessor.highBandComp.getRmsLevelInputDb(),
        audioProcessor.highBandComp.getRmsLevelOutputDb(),
    };
    
    analyzer.update(values);
    updateGlobalBypassButton();
}

void SimpleMBCompAudioProcessorEditor::updateGlobalBypassButton()
{
    auto params = getBypassParams();
    bool allBandsAreBypassed = std::all_of(
                                           params.begin(),
                                           params.end(),
                                           [](const auto& param) { return param->get(); }
                                           );
    controlBar.globalBypassButton.setToggleState(allBandsAreBypassed, juce::NotificationType::dontSendNotification);
}

void SimpleMBCompAudioProcessorEditor::toggleGlobalBypassState()
{
    auto shouldEnableEverything = ! controlBar.globalBypassButton.getToggleState();
    
    auto params = getBypassParams();
    
    auto bypassParamHelper = [](auto* param, bool shouldBeBypassed)
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(shouldBeBypassed ? 1.f : 0.f );
        param->endChangeGesture();
    };
    
    for( auto* param : params )
    {
        bypassParamHelper(param, ! shouldEnableEverything);
    }
    
    bandControls.toggleAllBands(! shouldEnableEverything);
}

std::array<juce::AudioParameterBool*, 3> SimpleMBCompAudioProcessorEditor::getBypassParams()
{
    using namespace Params;
    using namespace juce;
    const auto& params = Params::GetParams();
    auto& apvts = audioProcessor.apvts;
    
    auto boolHelper = [&apvts, &params](const auto& paramName)
    {
        auto param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
        
        return param;
    };
    auto* lowBypassParam = boolHelper(Names::Bypassed_Low_Band);
    auto* midBypassParam = boolHelper(Names::Bypassed_Mid_Band);
    auto* highBypassParam = boolHelper(Names::Bypassed_High_Band);
    return {
        lowBypassParam,
        midBypassParam,
        highBypassParam
    };
}


// Leaving off at *
