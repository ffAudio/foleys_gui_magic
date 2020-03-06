/*
 ==============================================================================
    Copyright (c) 2019 Foleys Finest Audio Ltd. - Daniel Walz
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.
 ==============================================================================
 */


namespace foleys
{


float XYDragComponent::radius = 4.0f;

XYDragComponent::XYDragComponent (juce::AudioProcessorValueTreeState& state)
  : xAttachment (state),
    yAttachment (state)
{
    setOpaque (false);

    setColour (xyDotColourId,            juce::Colours::orange.darker());
    setColour (xyDotOverColourId,        juce::Colours::orange);
    setColour (xyHorizontalColourId,     juce::Colours::orange.darker());
    setColour (xyHorizontalOverColourId, juce::Colours::orange);
    setColour (xyVerticalColourId,       juce::Colours::orange.darker());
    setColour (xyVerticalOverColourId,   juce::Colours::orange);

    xAttachment.onParameterChangedAsync = [&] { repaint(); };
    yAttachment.onParameterChangedAsync = [&] { repaint(); };
}

/**
 This sets the crosshair lines on and off.

 @param horizontal switches the horizontal line visible and enables vertical drags
 @param vertical   switches the vertical line visible and enables horizontal drags
 */
void XYDragComponent::setCrossHair (bool horizontal, bool vertical)
{
    wantsVerticalDrag = horizontal;
    wantsHorizontalDrag = vertical;
}

void XYDragComponent::paint (juce::Graphics& g)
{
    const auto x = getXposition();
    const auto y = getYposition();
    const auto gap = radius * 1.8f;

    if (wantsVerticalDrag)
    {
        g.setColour (findColour (mouseOverY ? xyVerticalOverColourId : xyVerticalColourId));
        if (x > gap)
            g.fillRect (0.0f, y - 1.0f, x - gap, 2.0f);

        if (x < getRight() - gap)
            g.fillRect (x + gap, y - 1.0f, getRight() - (x + gap), 2.0f);
    }

    if (wantsHorizontalDrag)
    {
        g.setColour (findColour (mouseOverX ? xyHorizontalOverColourId : xyHorizontalColourId));
        if (y > gap)
            g.fillRect (x - 1.0f, 0.0f, 2.0f, y - gap);

        if (y < getBottom() - gap)
            g.fillRect (x - 1.0f, y + gap, 2.0f, getBottom() - (y + gap));
    }

    g.setColour (findColour (mouseOverDot ? xyDotOverColourId : xyDotColourId));
    g.fillEllipse (x - radius, y - radius, 2 * radius, 2 * radius);
}

void XYDragComponent::setParameterX (const juce::String& paramID)
{
    xAttachment.attachToParameter (paramID);
}

void XYDragComponent::setParameterY (const juce::String& paramID)
{
    yAttachment.attachToParameter (paramID);
}

void XYDragComponent::updateWhichToDrag (juce::Point<float> pos)
{
    const auto centre = juce::Point<int> (getXposition(), getYposition()).toFloat();

    mouseOverDot = (centre.getDistanceFrom (pos) < radius * 1.5f);
    mouseOverX = (wantsHorizontalDrag && std::abs (pos.getX() - centre.getX()) < 3.0f);
    mouseOverY =  (wantsVerticalDrag && std::abs (pos.getY() - centre.getY()) < 3.0f);

    repaint();
}

bool XYDragComponent::hitTest (int x, int y)
{
    const auto click = juce::Point<int> (x, y).toFloat ();
    const auto centre = juce::Point<int> (getXposition (), getYposition ()).toFloat ();

    if (centre.getDistanceFrom (click) < radius * 1.5f)
        return true;

    if (wantsHorizontalDrag && std::abs (click.getX() - centre.getX()) < 3.0f)
        return true;

    if (wantsVerticalDrag && std::abs (click.getY() - centre.getY()) < 3.0f)
        return true;

    return false;
}

void XYDragComponent::mouseDown (const juce::MouseEvent& event)
{
    updateWhichToDrag (event.position);

    if (mouseOverX || mouseOverDot)
        xAttachment.beginGesture();

    if (mouseOverY || mouseOverDot)
        yAttachment.beginGesture();
}

void XYDragComponent::mouseMove (const juce::MouseEvent& event)
{
    updateWhichToDrag (event.position);
}

void XYDragComponent::mouseDrag (const juce::MouseEvent& event)
{
    if (mouseOverX || mouseOverDot)
        xAttachment.setNormalisedValue (event.position.getX() / float (getWidth()));

    if (mouseOverY || mouseOverDot)
        yAttachment.setNormalisedValue (1.0f - event.position.getY() / float (getHeight()));
}

void XYDragComponent::mouseUp (const juce::MouseEvent&)
{
    if (mouseOverX || mouseOverDot)
        xAttachment.endGesture();

    if (mouseOverY || mouseOverDot)
        yAttachment.endGesture();
}

void XYDragComponent::mouseEnter (const juce::MouseEvent& event)
{
    updateWhichToDrag (event.position);
}

void XYDragComponent::mouseExit (const juce::MouseEvent&)
{
    mouseOverDot = false;
    mouseOverX = false;
    mouseOverY = false;

    repaint();
}

int XYDragComponent::getXposition() const
{
    return juce::roundToInt (xAttachment.getNormalisedValue() * getWidth());
}

int XYDragComponent::getYposition() const
{
    return juce::roundToInt ((1.0f - yAttachment.getNormalisedValue()) * getHeight());
}

} // namespace foleys
