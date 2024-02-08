/*
 ==============================================================================
 
 This file is part of the JUCE tutorials.
 Copyright (c) 2020 - Raw Material Software Limited
 
 The code included in this file is provided under the terms of the ISC license
 http://www.isc.org/downloads/software-support-policy/isc-license. Permission
 To use, copy, modify, and/or distribute this software for any purpose with or
 without fee is hereby granted provided that the above copyright notice and
 this permission notice appear in all copies.
 
 THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
 WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
 PURPOSE, ARE DISCLAIMED.
 
 ==============================================================================
 */

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.
 
 BEGIN_JUCE_PIP_METADATA
 
 name:             LookAndFeelCustomisationTutorial
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Explores look and feel customisations.
 
 dependencies:     juce_core, juce_data_structures, juce_events, juce_graphics,
 juce_gui_basics
 exporters:        xcode_mac, vs2019, linux_make, xcode_iphone, androidstudio
 
 type:             Component
 mainClass:        MainContentComponent
 
 useLocalCopy:     1
 
 END_JUCE_PIP_METADATA
 
 *******************************************************************************/


#pragma once

class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        /*!
         sliderPos is a RATIO of how complete the slider's position is through 360 degrees. So halfway would be 0.5
         rotaryStartAngle and end angle are like what degree in radians the thing starts and ends at.
         */
        /*! colors */
        const auto outline = juce::Colours::palevioletred;
        const auto fill    = juce::Colours::blue;
        const auto text    = juce::Colours::black;
        const auto backgroundArcColor = juce::Colours::red;
        
        /* reduced by 10? */
        auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10);
        auto centre = bounds.getCentre();
        
        // no idea what this is. it doesnt change anything
//        const auto tickHeight = 17;
//        const auto tickWidth  = 40;
        
        // the radius of the circle. We get it buy asking the bounding box for it's width and height.
        // the bounding box is ideally a square but if not, the circle would not be perfectly centered on the x and y plane
        // for this reason we need to take the minimum of height and width of the bounding box. Imagine a skinny rectangle standing
        // on one of its slimmer sides.
        auto radius = juce::jmin (bounds.getWidth() / 2.0f, bounds.getHeight() / 2.0f);
        
        // not sure which thing this is for
        g.setColour (text);
        
        // ??? its already here dode ^^^^
        radius = juce::jmin (bounds.getWidth() / 2.0f, bounds.getHeight() / 2.0f);
        centre = bounds.getCentre();

        //evenly disperse 9 or any number of notches around the knob
        //        if (radius > 50.0f)
        //        {
        //            for (int i = 0; i < 9; ++i)
        //            {
        //                const auto angle = juce::jmap (i / 8.0f, rotaryStartAngle, rotaryEndAngle);
        //                const auto point = centre.getPointOnCircumference (radius - 2.0f, angle);
        //                g.fillEllipse (point.getX() - 3, point.getY() - 3, 6, 6);
        //            }
        //            radius -= 10.0f;
        //        }
        
        /* start angle, the start angle is */
        auto sliderPosInRadians = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = juce::jmin (4.0f, radius * 0.5f);
        auto arcRadius  = radius - lineW;
        
        juce::Path backgroundArc;
        backgroundArc.addCentredArc (bounds.getCentreX(),
                                     bounds.getCentreY(),
                                     arcRadius,
                                     arcRadius,
                                     0.0f,
                                     rotaryStartAngle,
                                     rotaryEndAngle,
                                     true);
        
        g.setColour (backgroundArcColor);
        g.strokePath (backgroundArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::butt));
        
        auto knobRadius = std:: max (radius - 3.0f * lineW, 10.0f);
        {
            juce::Graphics::ScopedSaveState saved (g);
            if (slider.isEnabled())
            {
                juce::ColourGradient fillGradient (outline.brighter(), centre.getX() + lineW * 2.0f, centre.getY() - lineW * 4.0f, outline, centre.getX() + knobRadius, centre.getY() + knobRadius, true);
                g.setGradientFill (fillGradient);
            }
            g.fillEllipse (centre.getX() - knobRadius, centre.getY() - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);
        }
        
        knobRadius = std:: max (knobRadius - 4.0f, 10.0f);
        g.setColour (outline.brighter());
        g.drawEllipse (centre.getX() - knobRadius, centre.getY() - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 2.0f);
        
        if (slider.isEnabled() && arcRadius > 10.0f)
        {
            juce::Path valueArc;
            valueArc.addCentredArc (bounds.getCentreX(),
                                    bounds.getCentreY(),
                                    arcRadius,
                                    arcRadius,
                                    0.0f,
                                    rotaryStartAngle,
                                    sliderPosInRadians,
                                    true);
            
            g.setColour (fill);
            g.strokePath (valueArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::butt));
        }
        
        juce::Path p;
        p.startNewSubPath (centre.getPointOnCircumference (knobRadius - lineW, sliderPosInRadians));
        p.lineTo (centre.getPointOnCircumference ((knobRadius - lineW) * 0.6f, sliderPosInRadians));
        g.strokePath (p, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        
    }
    
    OtherLookAndFeel()
    {
        setColour (juce::Slider::thumbColourId, juce::Colours::red);
    }
    
    
};

//==============================================================================
class MainContentComponent   : public juce::Component
{
public:
    MainContentComponent()
    {
        dial1.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        dial1.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible (dial1);
        dial1.setLookAndFeel(&otherLookAndFeel);
        
        dial2.setSliderStyle (juce::Slider::Rotary);
        dial2.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible (dial2);
        
        button1.setButtonText ("Button 1");
        addAndMakeVisible (button1);
        
        button2.setButtonText ("Button 2");
        addAndMakeVisible (button2);
        
//        We will use this binary image in memory as the drop shadow
        shadowImage = juce::ImageCache::getFromMemory (BinaryData::Slider_png, BinaryData::Slider_pngSize);

        
        setSize (300, 200);
        
    }
    
    ~MainContentComponent()
    {
        
        setLookAndFeel (nullptr);
        
    }
    
    void paint (juce::Graphics& g) override
    {
        
        g.fillAll(juce::Colours::lightgrey);
           g.drawImageWithin(shadowImage, dial1.getX(),
                             dial1.getY(), dial1.getWidth(), dial1.getHeight(),
                              juce::RectanglePlacement::centred);

           
        
    }
    
    void resized() override
    {
        auto border = 10;
        
        auto area = getLocalBounds();
        
        auto dialArea = area.removeFromTop (area.getHeight() / 3);
        dial1.setBounds (dialArea.removeFromLeft (dialArea.getWidth() / 2).reduced (border));
        dial2.setBounds (dialArea.reduced (border));
        
        auto buttonHeight = 30;
        
        button1.setBounds (area.removeFromTop (buttonHeight).reduced (border));
        button2.setBounds (area.removeFromTop (buttonHeight).reduced (border));
        
    }
    
private:
    OtherLookAndFeel otherLookAndFeel;
    juce::Slider dial1;
    juce::Slider dial2;
    juce::TextButton button1;
    juce::TextButton button2;
    // drop shadow png
    juce::Image shadowImage;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
