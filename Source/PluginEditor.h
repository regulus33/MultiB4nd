/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
 */

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics&,
                           int x, int y, int widt1h, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider&) override;
    
    void drawToggleButton (juce::Graphics &g,
                           juce::ToggleButton & toggleButton,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;
};

// Because we dynamically switch between params with this knob (for multiband)
// we store the param as a pointer and dereference them when we use them.
// DOC: Super class, we inherit from this in at least 1 place
struct RotarySliderWithLabels : juce::Slider
{
    // NOTE: the pointer here * is clever. If it were a & reference we would not be able
    // to DYNAMICALLY switch which band we are pointing to. A pointer is just a memory address so its quite easy to just replace it with some other address without creating a new UI component or creating a new reference. G whiz!
    RotarySliderWithLabels(juce::RangedAudioParameter* rap,
                           const juce::String& unitSuffix,
                           const juce::String& title
                           ) :
    juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                 juce::Slider::TextEntryBoxPosition::NoTextBox),
    param(rap),
    suffix(unitSuffix)
    {
        setName(title);
//        setLookAndFeel(&lnf);
    }
    
//    ~RotarySliderWithLabels()
//    {
//        setLookAndFeel(nullptr);
//    }
    
    struct LabelPos
    {
        float pos;
        juce::String label;
    };
    
    juce::Array<LabelPos> labels;
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    // DOC: virtual makes it overrideable
    virtual juce::String getDisplayString() const;
    
    void changeParam(juce::RangedAudioParameter* p);
    // DOC: we need to make private -> protected so the derrived class can access them.
protected:
//    LookAndFeel lnf;
    juce::RangedAudioParameter* param;
    juce::String suffix;
};

struct RatioSlider : RotarySliderWithLabels
{
    RatioSlider(juce::RangedAudioParameter* rap,
                const juce::String& unitSuffix) :
    RotarySliderWithLabels(rap, unitSuffix, "RATIO") {}
    
    juce::String getDisplayString() const override;
};

struct PowerButton : juce::ToggleButton { };

struct AnalyzerButton : juce::ToggleButton
{
    void resized() override
    {
        auto bounds = getLocalBounds();
        auto insetRect = bounds.reduced(4);
        
        randomPath.clear();
        
        juce::Random r;
        
        randomPath.startNewSubPath(insetRect.getX(),
                                   insetRect.getY() + insetRect.getHeight() * r.nextFloat());
        
        for( auto x = insetRect.getX() + 1; x < insetRect.getRight(); x += 2 )
        {
            randomPath.lineTo(x,
                              insetRect.getY() + insetRect.getHeight() * r.nextFloat());
        }
    }
    
    juce::Path randomPath;
};
// =========================================================
struct Placeholder : juce::Component
{
    Placeholder();
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(customColor);
    }
    
    juce::Colour customColor;
};

struct RotarySlider : public juce::Slider
{
    RotarySlider() :juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                 juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
        
    }
};

template<
typename Attachment,
typename APVTS,
typename Params,
typename ParamName,
typename SliderType
>

void makeAttachment(std::unique_ptr<Attachment>& attachment,
                    APVTS& apvts,
                    const Params& params,
                    const ParamName& name,
                    SliderType& slider)
{
    attachment = std::make_unique<Attachment>(apvts, params.at(name), slider);
}

template<
typename APVTS,
typename Params,
typename Name
>
juce::RangedAudioParameter& getParam(APVTS& apvts, const Params& params, const Name& name)
{
    auto param = apvts.getParameter(params.at(name));
    jassert(param != nullptr);
    
    return *param;
}

juce::String getValString(const juce::RangedAudioParameter& param, bool getLow, juce::String suffix);

template<
typename Labels,
typename ParamType,
typename SuffixType
>
void addLabelPairs(Labels& labels, const ParamType& param, const SuffixType& suffix)
{
    labels.clear();
    labels.add({0.f, getValString(param, true, suffix)});
    labels.add({1.f, getValString(param, false, suffix)});
}

struct CompressorBandControls : juce::Component, juce::Button::Listener
{
    CompressorBandControls(juce::AudioProcessorValueTreeState& apvts);
    ~CompressorBandControls() override;
    void resized() override;
    void paint(juce::Graphics& g) override;
    void buttonClicked(juce::Button* button) override;
private:
    juce::AudioProcessorValueTreeState&  apvts;
    
    RotarySliderWithLabels attackSlider, releaseSlider, thresholdSlider;
    RatioSlider ratioSlider;
    
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> attackSliderAttachment,
                                releaseSliderAttachment,
                                thresholdSliderAttachment,
                                ratioSliderAttachment;
    
    juce::ToggleButton bypassButton, soloButton, muteButton, lowBand, midBand, highBand;
    
    using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<BtnAttachment> bypassButtonAttachment,
                                   soloButtonAttachment,
                                    muteButtonAttachment;
    
    juce::Component::SafePointer<CompressorBandControls> safePtr {this};
    
    void updateAttachments();
    void updateSliderEnabledMents();
    void updateSoloMuteBypassToggleStates(juce::Button& button);
};

struct GlobalControls : juce::Component
{
    GlobalControls(juce::AudioProcessorValueTreeState& apvts);
    
    void paint(juce::Graphics& g) override;
    
    void resized() override;
private:
    using RSWL = RotarySliderWithLabels;
    std::unique_ptr<RSWL> inGainSlider, lowMidXoverSlider, midHighXoverSlider, outGainSlider;
    
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    std::unique_ptr<Attachment> lowMidXoverSliderAttachment,
    midHighXoverSliderAttachment,
    inGainSliderAttachment,
    outGainSliderAttachment;
};

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
