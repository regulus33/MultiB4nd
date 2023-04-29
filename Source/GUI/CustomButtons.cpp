/*
  ==============================================================================

    CustomButtons.cpp
    Created: 14 Apr 2023 7:31:05pm
    Author:  zack

  ==============================================================================
*/

#include "CustomButtons.h"

/*!

@brief Resizes the AnalyzerButton component
This method updates the shape of the AnalyzerButton component by creating a new random path.
The new path starts at a random point on the left side of the component's bounds, and creates
a series of line segments to random points on the right side of the bounds.
*/
void AnalyzerButton::resized()
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
