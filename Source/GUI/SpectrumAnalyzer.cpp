/*
 ==============================================================================
 
 SpectrumAnalyzer.cpp
 Created: 17 Apr 2023 6:15:34pm
 Author:  zack
 
 ==============================================================================
 */

#include "SpectrumAnalyzer.h"

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
    
    drawBackgroundGrid(g);
    
    auto responseArea = getAnalysisArea();
    
    if( shouldShowFFTAnalysis )
    {
        auto leftChannelFFTPath = leftPathProducer.getPath();
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
    
    g.setColour(Colours::white);
    //    g.strokePath(responseCurve, PathStrokeType(2.f));
    
    Path border;
    
    border.setUsingNonZeroWinding(false);
    
    border.addRoundedRectangle(getRenderArea(), 4);
    border.addRectangle(getLocalBounds());
    
    g.setColour(Colours::black);
    
    g.fillPath(border);
    
    drawTextLabels(g);
    
    g.setColour(Colours::orange);
    g.drawRoundedRectangle(getRenderArea().toFloat(), 4.f, 1.f);
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
    return std::vector<float>
    {
        -24, -12, 0, 12, 24
    };
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
void SpectrumAnalyzer::drawBackgroundGrid(juce::Graphics &g)
{
    using namespace juce;
    auto freqs = getFrequencies();
    
    auto renderArea = getAnalysisArea();
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
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));
        
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
void SpectrumAnalyzer::drawTextLabels(juce::Graphics &g)
{
    using namespace juce;
    g.setColour(Colours::lightgrey);
    const int fontHeight = 10;
    g.setFont(fontHeight);
    
    auto renderArea = getAnalysisArea();
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
        r.setY(1);
        
        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }
    
    auto gain = getGains();
    
    for( auto gDb : gain )
    {
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));
        
        String str;
        if( gDb > 0 )
            str << "+";
        str << gDb;
        
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        
        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setX(getWidth() - textWidth);
        r.setCentre(r.getCentreX(), y);
        
        g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::lightgrey );
        
        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
        
        str.clear();
        str << (gDb - 24.f);
        
        r.setX(1);
        textWidth = g.getCurrentFont().getStringWidth(str);
        r.setSize(textWidth, fontHeight);
        g.setColour(Colours::lightgrey);
        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
    }
}

void SpectrumAnalyzer::resized()
{
    using namespace juce;
    auto fftBounds = getAnalysisArea().toFloat();
    auto negInf = jmap(getLocalBounds().toFloat().getBottom(), fftBounds.getBottom(), fftBounds.getY(), -48.f, 0.f);
    
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
        auto fftBounds = getAnalysisArea().toFloat();
        fftBounds.setBottom(getLocalBounds().getBottom());
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
juce::Rectangle<int> SpectrumAnalyzer::getRenderArea()
{
    auto bounds = getLocalBounds();
    
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
juce::Rectangle<int> SpectrumAnalyzer::getAnalysisArea()
{
    auto bounds = getRenderArea();
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    return bounds;
}
