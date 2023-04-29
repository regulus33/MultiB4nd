/*
 ==============================================================================
 
 RotarySliderWithLabels.h
 Created: 14 Apr 2023 7:26:34pm
 Author:  zack
 
 ==============================================================================
 */

#pragma once
#include <JuceHeader.h>

/*!
 @struct RotarySliderWithLabels
 @brief A rotary slider with labels.
 RotarySliderWithLabels extends the JUCE Slider class to provide a rotary slider with labels. The labels can be customized and the slider can be associated with a JUCE audio parameter.
 */
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


/*!
 @struct RatioSlider
 @brief A ratio slider.
 RatioSlider extends the RotarySliderWithLabels class to provide a ratio slider. This slider is associated with a JUCE audio parameter and the display string is a ratio with a fixed 1:1 format.
 */
struct RatioSlider : RotarySliderWithLabels
{
    RatioSlider(juce::RangedAudioParameter* rap,
                const juce::String& unitSuffix) :
    RotarySliderWithLabels(rap, unitSuffix, "RATIO") {}
    
    juce::String getDisplayString() const override;
};
