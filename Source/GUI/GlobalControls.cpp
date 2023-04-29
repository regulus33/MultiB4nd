/*
 ==============================================================================
 
 GlobalControls.cpp
 Created: 14 Apr 2023 7:54:06pm
 Author:  zack
 
 ==============================================================================
 */

#include "GlobalControls.h"
#include "RotarySliderWithLabels.h"
#include "../DSP/Params.h"
#include "Utils.h"
#include "CustomButtons.h"

/*!
 The constructor for the GlobalControls class.
 This constructor creates 4 sliders (input gain, low-mid crossover frequency, mid-high crossover frequency, output gain) and attaches them to the corresponding parameters in the AudioProcessorValueTreeState object.
 The sliders are also given labels and label values based on the AudioParameter objects they are attached to.
 The sliders are added to the component and made visible.
 @param apvts An AudioProcessorValueTreeState object that holds the parameters used by the sliders.
 */
GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState& apvts)
{
    using namespace Params;
    const auto& params = GetParams();
    
    auto getParameterHelper = [&params, &apvts](const auto& name) -> auto&
    {
        return getParam(apvts, params, name);
    };
    
    
    auto& gainInParam = getParameterHelper(Names::Gain_In);
    auto& lowMidParam = getParameterHelper(Names::Low_Mid_Crossover_Freq);
    auto& midHighParam = getParameterHelper(Names::Mid_High_Crossover_Freq);
    auto& gainOutParam = getParameterHelper(Names::Gain_Out);
    
    inGainSlider = std::make_unique<RSWL>(&gainInParam, "dB", "INPUT TRIM");
    lowMidXoverSlider = std::make_unique<RSWL>(&lowMidParam, "Hz", "LOW-MID X-OVER");
    midHighXoverSlider = std::make_unique<RSWL>(&midHighParam, "Hz", "MID-HI X-OVER");
    outGainSlider = std::make_unique<RSWL>(&gainOutParam, "dB", "OUTPUT TRIM");
    
    auto makeAttachmentHelper = [&params, &apvts](auto& attachment,
                                                  const auto& name,
                                                  auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };
    
    makeAttachmentHelper(inGainSliderAttachment,
                         Names::Gain_In,
                         *inGainSlider);
    makeAttachmentHelper(lowMidXoverSliderAttachment,
                         Names::Low_Mid_Crossover_Freq,
                         *lowMidXoverSlider);
    makeAttachmentHelper(midHighXoverSliderAttachment,
                         Names::Mid_High_Crossover_Freq,
                         *midHighXoverSlider);
    makeAttachmentHelper(outGainSliderAttachment,
                         Names::Gain_Out,
                         *outGainSlider);
    
    addLabelPairs(inGainSlider->labels, gainInParam, "dB");
    
    addLabelPairs(lowMidXoverSlider->labels, lowMidParam, "hz");
    
    addLabelPairs(midHighXoverSlider->labels, midHighParam, "hz");
    
    addLabelPairs(outGainSlider->labels, gainOutParam, "dB");
    
    addAndMakeVisible(*inGainSlider);
    addAndMakeVisible(*lowMidXoverSlider);
    addAndMakeVisible(*midHighXoverSlider);
    addAndMakeVisible(*outGainSlider);
}

/*!
 @brief Resizes the GlobalControls component
 This method is responsible for laying out the child components of the GlobalControls component. The child components are sliders
 for gain input, low-mid crossover frequency, mid-high crossover frequency, and gain output.
 A FlexBox layout is used to arrange the sliders in a row. The order in which the sliders are added to the FlexBox determines the
 order in which they are displayed. The layout is performed by calling the performLayout() method of the FlexBox with the bounds
 of the GlobalControls component reduced by 5 pixels.

 */
void GlobalControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    using namespace juce;
    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    
    auto spacer = FlexItem().withWidth(4);
    auto endCap = FlexItem().withWidth(6);
    
    // What you are seeing here is in practice something like html or jsx in that the order we add these in is the order in which they are displayed
    flexBox.items.add(endCap);
    flexBox.items.add(FlexItem(*inGainSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*lowMidXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*midHighXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*outGainSlider).withFlex(1.f));
    flexBox.items.add(endCap);
    
    flexBox.performLayout(bounds);
}

/*!
@brief Renders the background of the GlobalControls component
This function is responsible for painting the background of the GlobalControls component.
It draws a rectangle with a slightly rounded border and a gradient fill.
@param g The Graphics object used for rendering
*/
void GlobalControls::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}
