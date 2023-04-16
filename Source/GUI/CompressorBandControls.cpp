/*
  ==============================================================================

    CompressorBandControls.cpp
    Created: 14 Apr 2023 7:50:49pm
    Author:  zack

  ==============================================================================
*/

#include "CompressorBandControls.h"
#include "RotarySliderWithLabels.h"
#include "../DSP/Params.h"
#include "Utils.h"

CompressorBandControls::CompressorBandControls(juce::AudioProcessorValueTreeState& apv) : apvts(apv),
    attackSlider(nullptr, "ms", "ATTACK"),
    releaseSlider(nullptr, "ms", "RELEASE"),
    thresholdSlider(nullptr, "dB", "THRESH"),
    ratioSlider(nullptr, "")
{

    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ratioSlider);
    
    // For changing colors of low, mid, high based on mute, solo, bypass
    bypassButton.addListener(this);
    soloButton.addListener(this);
    muteButton.addListener(this);
    
    using namespace juce::Colours;
    
    bypassButton.setName("X");
    bypassButton.setColour(ButtonOnColorId, yellow);
    bypassButton.setColour(ButtonOffColorId, black);
    
    soloButton.setName("S");
    soloButton.setColour(ButtonOnColorId, limegreen);
    soloButton.setColour(ButtonOffColorId, black);
    
    muteButton.setName("M");
    muteButton.setColour(ButtonOnColorId, red);
    muteButton.setColour(ButtonOffColorId, black);
    
    addAndMakeVisible(bypassButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(muteButton);
    
    lowBand.setName("Low");
    lowBand.setColour(ButtonOnColorId, grey);
    lowBand.setColour(ButtonOffColorId, black);
    
    midBand.setName("Mid");
    midBand.setColour(ButtonOnColorId, grey);
    midBand.setColour(ButtonOffColorId, black);
    
    highBand.setName("High");
    highBand.setColour(ButtonOnColorId, grey);
    highBand.setColour(ButtonOffColorId, black);
    
    lowBand.setRadioGroupId(1);
    midBand.setRadioGroupId(1);
    highBand.setRadioGroupId(1);
    
    // Switching global band =====================
    auto buttonSwitcher = [safePtr = this->safePtr]()
    {
      if(auto* c = safePtr.getComponent())
      {
          c->updateAttachments();
      }
    };
    
    lowBand.onClick = buttonSwitcher;
    midBand.onClick = buttonSwitcher;
    highBand.onClick = buttonSwitcher;
    
    // NOTE: we don't want to trigger the onClick = lambda yet, so we use "donSendNotification"
    lowBand.setToggleState(true, juce::NotificationType::dontSendNotification);
    updateAttachments();
    // ===========================================
    
    addAndMakeVisible(lowBand);
    addAndMakeVisible(midBand);
    addAndMakeVisible(highBand);
}

CompressorBandControls::~CompressorBandControls()
{
    bypassButton.removeListener(this);
    soloButton.removeListener(this);
    muteButton.removeListener(this);
    
}

void CompressorBandControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    using namespace juce;
    
    std::function<FlexBox(std::vector<Component*>)> createButtonControlBox = [](std::vector<Component*> comps) -> FlexBox {
        FlexBox flexBox;
        flexBox.flexDirection = FlexBox::Direction::column;
        flexBox.flexWrap = FlexBox::Wrap::noWrap;
        
        auto spacer = FlexItem().withHeight(4);
        
        for( auto* comp : comps)
        {
            flexBox.items.add(spacer);
            flexBox.items.add(FlexItem(*comp).withFlex(1.f));
        }
        
        flexBox.items.add(spacer);
        
        return flexBox;
    };
    
    // pass vector of pointers to ui components, the lambda will organize them into a flexbox
    auto bandButtonControlBox = createButtonControlBox({&bypassButton, &soloButton, &muteButton});
    auto bandSelectControlBox = createButtonControlBox({&lowBand, &midBand, &highBand});
    
    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    
    auto spacer = FlexItem().withWidth(4);
//    auto endCap = FlexItem().withWidth(6);
    
    // What you are seeing here is in practice something like html or jsx in that the order we add these in is the order in which they are displayed
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(bandSelectControlBox).withWidth(50));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(attackSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(releaseSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(thresholdSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(ratioSlider).withFlex(1.f));
//    flexBox.items.add(endCap);
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(bandButtonControlBox).withWidth(30));
    
    flexBox.performLayout(bounds);
}



void CompressorBandControls::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

// ----
void CompressorBandControls::refreshBandButtonColors(juce::Button& band, juce::Button& colorSource)
{
    band.setColour(ButtonOnColorId, colorSource.findColour(ButtonOnColorId));
    band.setColour(ButtonOffColorId, colorSource.findColour(ButtonOnColorId));
    band.repaint();
}

void CompressorBandControls::resetActiveBandColors()
{
    activeBand->setColour(ButtonOnColorId, juce::Colours::grey);
    activeBand->setColour(ButtonOffColorId, juce::Colours::grey);
    activeBand->repaint();
}
// -----

// DOC: figure out which band is selected and then color it based on whether its muted, bypassed or soloed (only called on plugin startup, we spread this around in other callbacks later in plugin lifecycle
void CompressorBandControls::updateBandSelectButtonStates()
{
    using namespace Params;
    std::vector<std::array<Names, 3>> paramsToCheck
    {
        {Names::Solo_Low_Band, Names::Mute_Low_Band, Names::Bypassed_Low_Band},
        {Names::Solo_Mid_Band, Names::Mute_Mid_Band, Names::Bypassed_Mid_Band},
        {Names::Solo_High_Band, Names::Mute_High_Band, Names::Bypassed_High_Band},
    };
    
    const auto& params = GetParams();
    auto paramHelper = [&params, this](const auto& name)
    {
        return dynamic_cast<juce::AudioParameterBool*>(&getParam(apvts, params, name));
    };
    
    for(size_t i = 0; i < paramsToCheck.size(); ++i)
    {
        auto& list = paramsToCheck[i];
        
        auto* bandButton = (i == 0) ? &lowBand :
                           (i == 1) ? &midBand :
                                      &highBand;
        
        if( auto* solo = paramHelper(list[0]);
           solo->get() )
        {
            refreshBandButtonColors(*bandButton, soloButton);
        }
        
        else if( auto* mute = paramHelper(list[1]);
           mute->get() )
        {
            refreshBandButtonColors(*bandButton, muteButton);
        }
        
        else if( auto* bypass = paramHelper(list[2]);
           bypass->get() )
        {
            refreshBandButtonColors(*bandButton, bypassButton);
        }
        
    }
}

void CompressorBandControls::buttonClicked(juce::Button* button)
{
    updateSliderEnabledMents();
    updateSoloMuteBypassToggleStates(*button);
    updateActiveBandFillColors(*button);
}

void CompressorBandControls::updateActiveBandFillColors(juce::Button& clickedButton)
{
    jassert(activeBand != nullptr);
    DBG("Active band: " << activeBand->getName());
    
    if(clickedButton.getToggleState() == false)
    {
        resetActiveBandColors();
    }
    else
    {
        refreshBandButtonColors(*activeBand, clickedButton);
    }
}

void CompressorBandControls::updateSliderEnabledMents()
{
    auto disabled = muteButton.getToggleState() || bypassButton.getToggleState();
    attackSlider.setEnabled(!disabled);
    releaseSlider.setEnabled(!disabled);
    thresholdSlider.setEnabled(!disabled);
    ratioSlider.setEnabled(!disabled);
}

void CompressorBandControls::updateSoloMuteBypassToggleStates(juce::Button &clickedButton)
{
    // disable bypass and mute if solo selected
    if(&clickedButton == &soloButton && soloButton.getToggleState())
    {
        bypassButton.setToggleState(false, juce::NotificationType::sendNotification);
        muteButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
    
    // disable bypass and solo if mute selected
    else if(&clickedButton == &muteButton && muteButton.getToggleState())
    {
        bypassButton.setToggleState(false, juce::NotificationType::sendNotification);
        soloButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
    
    // disable mute and solo if bypass selected
    else if(&clickedButton == &bypassButton && bypassButton.getToggleState())
    {
        muteButton.setToggleState(false, juce::NotificationType::sendNotification);
        soloButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
}

void CompressorBandControls::updateAttachments()
{
    // Arbitrary band types
    // TODO: are these necessary? Can't we do the switch case without them?
    enum BandType
    {
        Low,
        Mid,
        High
    };
    // immediately invoked lambda, probably not necessary...
    BandType bandType = [this]()
    {
        if(lowBand.getToggleState())
            return BandType::Low;
        if(midBand.getToggleState())
            return BandType::Mid;
        
        return BandType::High;
    }();
    
    using namespace Params;
    std::vector<Names> names;
    
    // NOTE: this is a little clunky. we are both getting the Names::thing and also using its index to
    // determine the name AND the parameter itself since params all over this plugin can be predicted based on their order
    //    0 = attack
    //    1 = relase
    //    2 = threshold
    //    3 = ratio
    //    4 = mute
    //    5 = solo
    //    6 = bypass
    switch(bandType)
    {
            
        case Low:
            names = std::vector<Names>
            {
                Names::Attack_Low_Band,
                Names::Release_Low_Band,
                Names::Threshold_Low_Band,
                Names::Ratio_Low_Band,
                Names::Mute_Low_Band,
                Names::Solo_Low_Band,
                Names::Bypassed_Low_Band
            };
            activeBand = &lowBand;
            
            break;
        case Mid:
            names = std::vector<Names>
            {
                Names::Attack_Mid_Band,
                Names::Release_Mid_Band,
                Names::Threshold_Mid_Band,
                Names::Ratio_Mid_Band,
                Names::Mute_Mid_Band,
                Names::Solo_Mid_Band,
                Names::Bypassed_Mid_Band
            };
            activeBand = &midBand;
            
            break;
        case High:
            names = std::vector<Names>
            {
                Names::Attack_High_Band,
                Names::Release_High_Band,
                Names::Threshold_High_Band,
                Names::Ratio_High_Band,
                Names::Mute_High_Band,
                Names::Solo_High_Band,
                Names::Bypassed_High_Band
            };
            activeBand = &highBand;
            
            break;
    }
    
    enum Pos
    {
        Attack,
        Release,
        Threshold,
        Ratio,
        Mute,
        Solo,
        Bypass
    };
    
    const auto& params = GetParams();
    
    auto getParameterHelper = [&params, &apvts = this->apvts, &names](const auto& pos) -> auto&
    {
        return getParam(apvts, params, names.at(pos));
    };
    
    attackSliderAttachment.reset();
    releaseSliderAttachment.reset();
    thresholdSliderAttachment.reset();
    ratioSliderAttachment.reset();
    bypassButtonAttachment.reset();
    soloButtonAttachment.reset();
    muteButtonAttachment.reset();
    
    // We call getParam and try to get Attack of whatever band (based on pos which is 0)
    auto& attackParam = getParameterHelper(Pos::Attack);
    addLabelPairs(attackSlider.labels, attackParam, "ms");
    attackSlider.changeParam(&attackParam);
    
    auto& releaseParam = getParameterHelper(Pos::Release);
    addLabelPairs(releaseSlider.labels, releaseParam, "ms");
    releaseSlider.changeParam(&releaseParam);
    
    auto& thresholdParam = getParameterHelper(Pos::Threshold);
    addLabelPairs(thresholdSlider.labels, thresholdParam, "dB");
    thresholdSlider.changeParam(&thresholdParam);
    
//    auto& ratioParam = getParameterHelper(Pos::Ratio);
//    addLabelPairs(ratioSlider.labels, &ratioParam, "?");
//    ratioSlider.changeParam(&ratioParam);
    
    auto& ratioParamRap = getParameterHelper(Pos::Ratio);
    
    ratioSlider.labels.clear();
    auto ratioParam = dynamic_cast<juce::AudioParameterChoice*>(&ratioParamRap);
    int choicesEndIndex = ratioParam->choices.size() - 1;
    int finalRatioValue = ratioParam->choices.getReference(choicesEndIndex).getIntValue();
    juce::String endRatioLabel = juce::String(finalRatioValue) + ":1";
    ratioSlider.labels.add({1.0f, endRatioLabel});
    ratioSlider.changeParam(ratioParam);
    
    // TODO: this has been duplicated >= 3 times
    auto makeAttachmentHelper = [&params, &apvts = this->apvts](auto& attachment,
                                                  const auto& name,
                                                  auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };
    
    makeAttachmentHelper(attackSliderAttachment, names[Pos::Attack], attackSlider);
    makeAttachmentHelper(releaseSliderAttachment, names[Pos::Release], releaseSlider);
    makeAttachmentHelper(thresholdSliderAttachment, names[Pos::Threshold], thresholdSlider);
    makeAttachmentHelper(ratioSliderAttachment, names[Pos::Ratio], ratioSlider);
    makeAttachmentHelper(bypassButtonAttachment, names[Pos::Bypass], bypassButton);
    makeAttachmentHelper(soloButtonAttachment, names[Pos::Solo], soloButton);
    makeAttachmentHelper(muteButtonAttachment, names[Pos::Mute], muteButton);
}
