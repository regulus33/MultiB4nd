/*
 ==============================================================================
 
 SpectrumAnalyzer.cpp
 Created: 17 Apr 2023 6:15:34pm
 Author:  zack
 
 ==============================================================================
 */

#include "SpectrumAnalyzer.h"
#include "Utils.h"
#include "../DSP/Params.h"

/*!
 @brief Constructor for the SpectrumAnalyzer class.
 The constructor initializes the leftPathProducer and rightPathProducer with
 the leftChannelFifo and rightChannelFifo of the SimpleMBCompAudioProcessor instance.
 It also adds a listener to each parameter of the audio processor. Finally, it starts
 a timer with a frequency of 60 times per second.
 @param p - A reference to the SimpleMBCompAudioProcessor instance.
 */
SpectrumAnalyzer::SpectrumAnalyzer(SimpleMBCompAudioProcessor& p) :
audioProcessor(p),
leftPathProducer(audioProcessor.leftChannelFifo),
rightPathProducer(audioProcessor.rightChannelFifo)
{
    
    const juce::Array<juce::AudioProcessorParameter*>& params = audioProcessor.getParameters();
    for( auto param : params )
    {
        param->addListener(this);
    }
    
    using namespace Params;
    const auto& paramNames = GetParams();
    
    auto floatHelper = [&apvts = audioProcessor.apvts, &paramNames](auto& param, const auto& paramName)
    {
        // we have a reference to the param member var here and we set it to value from apvts
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(paramNames.at(paramName)));
        jassert(param != nullptr);
    };
    
    floatHelper(lowMidXoverParam, Names::Low_Mid_Crossover_Freq);
    floatHelper(midHighXoverParam, Names::Mid_High_Crossover_Freq);
    
    floatHelper(lowThresholdParam, Names::Threshold_Low_Band);
    floatHelper(midThresholdParam, Names::Threshold_Mid_Band);
    floatHelper(highThresholdParam, Names::Threshold_High_Band);

    
    // starts the timer with a frequency of 60x per second
    startTimerHz(60);
}

/*!
 @brief SpectrumAnalyzer destructor.
 Removes listener from all the parameters of the audio processor.
 */
SpectrumAnalyzer::~SpectrumAnalyzer()
{
    const juce::Array<juce::AudioProcessorParameter*>& params = audioProcessor.getParameters();
    for( auto param : params )
    {
        param->removeListener(this);
    }
}


void SpectrumAnalyzer::drawFFTAnalysis(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    using namespace juce;
    auto leftChannelFFTPath = leftPathProducer.getPath();
    auto responseArea = getAnalysisArea(bounds);
    
    /* Somehow this reduces the box size (there was a white line we wanted to cover over) */
    Graphics::ScopedSaveState sss(g);
    g.reduceClipRegion(responseArea);
    
    leftChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(),
                                                                    0
                                                                    //                                                                      responseArea.getY()
                                                                    ));
    
    g.setColour(Colour(97u, 18u, 167u)); //purple-
    g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));
    
    auto rightChannelFFTPath = rightPathProducer.getPath();
    rightChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(),
                                                                     0
                                                                     //                                                                         responseArea.getY()
                                                                     ));
    
    g.setColour(Colour(215u, 201u, 134u));
    g.strokePath(rightChannelFFTPath, PathStrokeType(1.f));
}

/*!
 @brief Renders the graphical components on the GUI.
 This method is called whenever a repaint is needed. It's responsible for
 rendering the background grid, the FFT analysis results, the response curve,
 the border and the text labels.
 @param g The Graphics object used to render the components.
 */
void SpectrumAnalyzer::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);
    
    auto bounds = drawModuleBackground(g, getLocalBounds());
    
    drawBackgroundGrid(g, bounds);
    
    auto responseArea = getAnalysisArea(bounds);
    
    if( shouldShowFFTAnalysis )
    {
        drawFFTAnalysis(g, bounds);

    }
    
    g.setColour(Colours::white);
    //    g.strokePath(responseCurve, PathStrokeType(2.f));
    
//    Path border;
//
//    border.setUsingNonZeroWinding(false);
//
//    border.addRoundedRectangle(getRenderArea(bounds), 4);
//    border.addRectangle(getLocalBounds());
//
//    g.setColour(Colours::black);
    
//    g.fillPath(border);
    
    drawCrossovers(g, bounds);
    drawTextLabels(g, bounds);
//    g.setColour(Colours::orange);
//    g.drawRoundedRectangle(getRenderArea(bounds).toFloat(), 4.f, 1.f);
}

void SpectrumAnalyzer::drawCrossovers(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    using namespace juce;
    bounds = getAnalysisArea(bounds);
    
    const auto top = bounds.getY();
    const auto bottom = bounds.getBottom();
    const auto left = bounds.getX();
    const auto right = bounds.getRight();
    
    
    auto mapX = [left = bounds.getX(), width = bounds.getWidth()](float frequency)
    {
        auto normX = juce::mapFromLog10(frequency, 20.f, 20000.f);
        return left + width * normX;
    };
    
    auto lowMidX = mapX(lowMidXoverParam->get());
    g.setColour(Colours::orange);
    g.drawVerticalLine(lowMidX, top, bottom);
    
    auto midHighX = mapX(midHighXoverParam->get());
    g.setColour(Colours::orange);
    g.drawVerticalLine(midHighX, top, bottom);
    
    auto mapY = [bottom, top](float db)
    {
        return jmap(db, NEGATIVE_INFINITY, MAX_DECIBALS, (float)bottom, (float)top);
    };
    
    g.setColour(Colours::yellow);
    
    g.drawHorizontalLine(mapY(lowThresholdParam->get()), left, lowMidX);
    g.drawHorizontalLine(mapY(midThresholdParam->get()), lowMidX, midHighX);
    g.drawHorizontalLine(mapY(highThresholdParam->get()), midHighX, right);
}


/*!
 @brief Generates the frequencies used for the spectrum analyzer visualization.
 This method generates the frequencies used for the spectrum analyzer visualization.
 The frequencies generated are` [20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000]`.
 @return std::vector<float> The generated frequencies.
 */
std::vector<float> SpectrumAnalyzer::getFrequencies()
{
    return std::vector<float>
    {
        20, /*30, 40,*/ 50, 100,
        200, /*300, 400,*/ 500, 1000,
        2000, /*3000, 4000,*/ 5000, 10000,
        20000
    };
}

/*!
 @brief Generates the gains used for the spectrum analyzer visualization.
 This method generates the gains used for the spectrum analyzer visualization.
 The gains generated are [-24, -12, 0, 12, 24].
 @return std::vector<float> The generated gains.
 */
std::vector<float> SpectrumAnalyzer::getGains()
{
//    return std::vector<float>
//    {
//        -24, -12, 0, 12, 24
//    };
    std::vector<float> values;
    
    auto increment = MAX_DECIBALS; // 12db steps
    for(auto db = NEGATIVE_INFINITY; db <= MAX_DECIBALS; db += increment)
    {
        values.push_back(db);
    }
    
    return values;
}

/*!
 @brief Calculates the x-coordinate of a frequency on the logarithmic frequency axis.
 This method maps the input frequencies from a logarithmic frequency scale to a linear x-coordinate scale using the juce::mapFromLog10 function. The x-coordinate is then calculated by multiplying the normalized x-coordinate by the width of the frequency axis and adding the left edge of the frequency axis. The resulting x-coordinates represent the position of each frequency on the frequency axis.
 @param freqs The frequencies to calculate the x-coordinate for.
 @param left The left edge of the frequency axis.
 @param width The width of the frequency axis.
 @return A vector of the x-coordinates for each frequency.
 */
std::vector<float> SpectrumAnalyzer::getXs(const std::vector<float> &freqs, float left, float width)
{
    std::vector<float> xs;
    for( auto f : freqs )
    {
        auto normX = juce::mapFromLog10(f, 20.f, 20000.f);
        xs.push_back( left + width * normX );
    }
    
    return xs;
}

/*!
 @brief Renders the graphical components on the GUI.
 This method is called whenever a repaint is needed. It's responsible for
 rendering the background grid, the FFT analysis results, the response curve,
 the border and the text labels.
 @param g The Graphics object used to render the components.
 */
void SpectrumAnalyzer::drawBackgroundGrid(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    using namespace juce;
    auto freqs = getFrequencies();
    
    auto renderArea = getAnalysisArea(bounds);
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();
    
    auto xs = getXs(freqs, left, width);
    
    g.setColour(Colours::dimgrey);
    for( auto x : xs )
    {
        g.drawVerticalLine(x, top, bottom);
    }
    
    auto gain = getGains();
    
    for( auto gDb : gain )
    {
//        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));
        auto y = jmap(gDb, NEGATIVE_INFINITY, MAX_DECIBALS, (float)bottom, (float)top);
        
        g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::darkgrey );
        g.drawHorizontalLine(y, left, right);
    }
}

/*!
 @brief sets the animation callback used to draw the audio curves.
 It will happen in a juce process at a frequesny we specify (60x per second for us by default)
 looks like this `timer->timerCallback();`
 @return void
 */
void SpectrumAnalyzer::drawTextLabels(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    using namespace juce;
    g.setColour(Colours::lightgrey);
    const int fontHeight = 10;
    g.setFont(fontHeight);
    
    auto renderArea = getAnalysisArea(bounds);
    auto left = renderArea.getX();
    
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();
    
    auto freqs = getFrequencies();
    auto xs = getXs(freqs, left, width);
    
    for( int i = 0; i < freqs.size(); ++i )
    {
        auto f = freqs[i];
        auto x = xs[i];
        
        bool addK = false;
        String str;
        if( f > 999.f )
        {
            addK = true;
            f /= 1000.f;
        }
        
        str << f;
        if( addK )
            str << "k";
        str << "Hz";
        
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        
        Rectangle<int> r;
        
        r.setSize(textWidth, fontHeight);
        r.setCentre(x, 0);
        r.setY(bounds.getY());
        
        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }
    
    auto gain = getGains();
    
    for( auto gDb : gain )
    {
//        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));
        auto y = jmap(gDb, NEGATIVE_INFINITY, MAX_DECIBALS, (float)bottom, (float)top);
        
        String str;
        if( gDb > 0 )
            str << "+";
        str << gDb;
        
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        
        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setX(bounds.getRight() - textWidth);
        r.setCentre(r.getCentreX(), y);
        
        g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::lightgrey );
        
        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
        
//        str.clear();
//        str << (gDb - 24.f);
        
        r.setX(bounds.getX() + 1);
//        textWidth = g.getCurrentFont().getStringWidth(str);
//        r.setSize(textWidth, fontHeight);
//        g.setColour(Colours::lightgrey);
        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
    }
}

void SpectrumAnalyzer::resized()
{
    using namespace juce;
    auto bounds = getLocalBounds();
    auto fftBounds = getAnalysisArea(bounds).toFloat();
    auto negInf = jmap(bounds.toFloat().getBottom(), fftBounds.getBottom(), fftBounds.getY(), NEGATIVE_INFINITY, MAX_DECIBALS);
    
    DBG("Negative infinity: " << negInf);
    leftPathProducer.updateNegativeInfinity(negInf);
    rightPathProducer.updateNegativeInfinity(negInf);
}

/*!
 @brief Callback method that is triggered when a parameter value has changed.
 This method is called by Juce when a parameter value has changed. The method sets
 the parametersChanged flag to true so that the timerCallback method will
 update the visual representation of the parameters.
 @param parameterIndex Index of the parameter that has changed
 @param newValue The new value of the parameter
 */
void SpectrumAnalyzer::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}

/*!
 @brief Creates callback to frame by frame for the spectrum analyzer
 The timerCallback method is called at a frequency determined by startTimerHz. It performs the following tasks:
 If shouldShowFFTAnalysis is true, it calls the process method on leftPathProducer and rightPathProducer, passing in the analysis area bounds and the sample rate of the audio processor.
 If parametersChanged is true, it sets it to false.
 It calls the repaint method to redraw the component.
 */
void SpectrumAnalyzer::timerCallback()
{
    if( shouldShowFFTAnalysis )
    {
        auto bounds = getLocalBounds();
        auto fftBounds = getAnalysisArea(bounds).toFloat();
        fftBounds.setBottom(bounds.getBottom());
        auto sampleRate = audioProcessor.getSampleRate();
        
        leftPathProducer.process(fftBounds, sampleRate);
        rightPathProducer.process(fftBounds, sampleRate);
    }
    
    if( parametersChanged.compareAndSetBool(false, true) )
    {
        
        
    }
    
    repaint();
}


/*!
 @brief Returns the area of the component in which the audio analysis will be rendered.
 The render area is defined as the bounds of the component with a top and bottom margin of 4 units,
 and a left and right margin of 20 units.
 @return juce::Rectangle<int> The area of the component for audio analysis rendering.
 */
juce::Rectangle<int> SpectrumAnalyzer::getRenderArea(juce::Rectangle<int> bounds)
{
//    auto bounds = getLocalBounds();
    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);
    
    return bounds;
}

/*!
 @brief Retrieves the analysis area of the spectrum analyzer.
 This method calculates the area where the analysis of the audio signal will be performed,
 by removing 4 pixels from the top and 4 pixels from the bottom of the render area of the
 spectrum analyzer.
 @return A rectangle representing the analysis area.
 */
juce::Rectangle<int> SpectrumAnalyzer::getAnalysisArea(juce::Rectangle<int> bounds)
{
    bounds = getRenderArea(bounds);
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    return bounds;
}
