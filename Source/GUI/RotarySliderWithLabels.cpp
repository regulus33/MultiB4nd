/*
  ==============================================================================

    RotarySliderWithLabels.cpp
    Created: 14 Apr 2023 7:26:34pm
    Author:  zack

  ==============================================================================
*/

#include "RotarySliderWithLabels.h"
#include "Utils.h"

/*!
@brief Paint method for the RotarySliderWithLabels component.
This method draws the rotary slider and the labels for the component. It uses the
juce::LookAndFeel_V4 method drawRotarySlider to render the slider, and draws the labels
by calculating the position of each label and calling the juce::Graphics method
drawFittedText to render each label.
@param g The juce Graphics context in which to draw the component.
*/
void RotarySliderWithLabels::paint(juce::Graphics &g)
{
    using namespace juce;
    
    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;
    
    auto range = getRange();
    
    auto sliderBounds = getSliderBounds();
    auto bounds = getLocalBounds();
    
    g.setColour(Colours::blueviolet);
    g.drawFittedText(getName(),
                     bounds.removeFromTop(getTextHeight() + 2),
                     Justification::centredBottom,
                     1);
//    g.setColour(Colours::red);
//    g.drawRect(getLocalBounds());
//    g.setColour(Colours::yellow);
//    g.drawRect(sliderBounds);
    
    getLookAndFeel().drawRotarySlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                      startAng,
                                      endAng,
                                      *this);
    
    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * 0.5f;
    
    g.setColour(Colour(0u, 172u, 1u));
    g.setFont(getTextHeight());
    
    auto numChoices = labels.size();
    for( int i = 0; i < numChoices; ++i )
    {
        auto pos = labels[i].pos;
        jassert(0.f <= pos);
        jassert(pos <= 1.f);
        
        auto ang = jmap(pos, 0.f, 1.f, startAng, endAng);
        
        auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, ang);
        
        Rectangle<float> r;
        auto str = labels[i].label;
        r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight());
        r.setCentre(c);
        r.setY(r.getY() + getTextHeight());
        
        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }
    
}

/*!
@brief Get the bounds of the slider component.
This method returns a juce::Rectangle<int> that represents the bounds of the slider component.
It calculates the size of the slider component based on the size of the component's bounds and
the text height, and centers the slider component within the component's bounds.
@return The bounds of the slider component.
*/
juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    
    bounds.removeFromTop(getTextHeight() * 1.5);
    
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    
    size -= getTextHeight() * 1.5;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
//    r.setY(2);
    r.setY(bounds.getY());
    
    return r;
    
}

/*!
@brief Get the display string for the component.
This method returns a juce::String that represents the display string for the component. If the
component's parameter is a juce::AudioParameterChoice, the method returns the name of the
current choice. If the component's parameter is a juce::AudioParameterFloat, the method
returns a string representation of the float value, with a suffix if one is provided.
@return The display string for the component.
*/
juce::String RotarySliderWithLabels::getDisplayString() const
{
    if( auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param) )
        return choiceParam->getCurrentChoiceName();
    
    juce::String str;
    bool addK = false;
    
    if( auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param) )
    {
        float val = getValue();
        
//        if( val > 999.f )
//        {
//            val /= 1000.f; //1001 / 1000 = 1.001
//            addK = true;
//        }
        addK = truncateKiloValue(val);
        str = juce::String(val, (addK ? 2 : 0));
    }
    else
    {
        jassertfalse; //this shouldn't happen!
    }
    
    if( suffix.isNotEmpty() )
    {
        str << " ";
        if( addK )
            str << "k";
        
        str << suffix;
    }
    
    return str;
}

/*!
@brief Change the parameter for the component.
This method changes the parameter for the component to a new juce::RangedAudioParameter
pointer. It then repaints the component.
@param p The new juce::RangedAudioParameter pointer.
*/
void RotarySliderWithLabels::changeParam(juce::RangedAudioParameter *p)
{
    param = p;
    repaint();
}

/*!
@brief Get the display string for a RatioSlider component.
This method returns a juce::String that represents the display string for a RatioSlider
component. It casts the component's parameter to a juce::AudioParameterChoice and returns
the current choice with ":1" appended to the end.
@return The display string for the RatioSlider component.
*/
juce::String RatioSlider::getDisplayString() const
{
    auto choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param);
    jassert(choiceParam != nullptr);
    
    auto currentChoice = choiceParam->getCurrentChoiceName();
    // clamp the substring that does not include .0 if
    if(currentChoice.contains(".0"))
    {
        currentChoice = currentChoice.substring(0, currentChoice.indexOf("."));
    }
    
    currentChoice << ":1";
    
    return currentChoice;
}
