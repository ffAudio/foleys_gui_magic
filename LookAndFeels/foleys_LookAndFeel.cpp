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

void LookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                    const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    const auto outline = slider.findColour (juce::Slider::rotarySliderOutlineColourId);
    const auto fill    = slider.findColour (juce::Slider::rotarySliderFillColourId);
    const auto text    = slider.findColour (juce::Slider::textBoxTextColourId);

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10);
    auto centre = bounds.getCentre();

    const auto tickHeight = 6;
    const auto tickWidth  = 40;

    auto radius = juce::jmin (bounds.getWidth() / 2.0f, bounds.getHeight() / 2.0f);

    g.setColour (text);
    if (radius > tickWidth * 2.0f + 10.0f)
    {
        const auto xLeft  = int (bounds.getX());
        const auto xRight = int (bounds.getRight() - tickWidth);
        g.drawFittedText (slider.getTextFromValue (slider.proportionOfLengthToValue (0.5f)),   int (centre.getX() - tickWidth / 2), int (bounds.getY()), tickWidth, tickHeight, juce::Justification::centred, 1);
        g.drawFittedText (slider.getTextFromValue (slider.proportionOfLengthToValue (0.375f)), xLeft,  int (bounds.getY()), tickWidth, tickHeight, juce::Justification::left, 1);
        g.drawFittedText (slider.getTextFromValue (slider.proportionOfLengthToValue (0.25f)),  xLeft,  int (juce::jmap (0.33f,  float (bounds.getY()), bounds.getBottom() - tickHeight)), tickWidth, tickHeight, juce::Justification::left, 1);
        g.drawFittedText (slider.getTextFromValue (slider.proportionOfLengthToValue (0.125f)), xLeft,  int (juce::jmap (0.66f,  float (bounds.getY()), bounds.getBottom() - tickHeight)), tickWidth, tickHeight, juce::Justification::left, 1);
        g.drawFittedText (slider.getTextFromValue (slider.proportionOfLengthToValue (0.0f)),   xLeft,  int (bounds.getBottom() - tickHeight), tickWidth, tickHeight, juce::Justification::left, 1);
        g.drawFittedText (slider.getTextFromValue (slider.proportionOfLengthToValue (0.625f)), xRight, int (bounds.getY()), tickWidth, tickHeight, juce::Justification::right, 1);
        g.drawFittedText (slider.getTextFromValue (slider.proportionOfLengthToValue (0.75f)),  xRight, int (juce::jmap (0.33f,  float (bounds.getY()), bounds.getBottom() - tickHeight)), tickWidth, tickHeight, juce::Justification::right, 1);
        g.drawFittedText (slider.getTextFromValue (slider.proportionOfLengthToValue (0.875f)), xRight, int (juce::jmap (0.66f,  float (bounds.getY()), bounds.getBottom() - tickHeight)), tickWidth, tickHeight, juce::Justification::right, 1);
        g.drawFittedText (slider.getTextFromValue (slider.proportionOfLengthToValue (1.0f)),   xRight, int (bounds.getBottom() - tickHeight), tickWidth, tickHeight, juce::Justification::right, 1);

        bounds.removeFromTop (tickHeight + 4.0f);
        bounds.reduce (tickWidth, 0.0f);
    }

    radius = juce::jmin (bounds.getWidth() / 2.0f, bounds.getHeight() / 2.0f);
    centre = bounds.getCentre();

    if (radius > 50.0f)
    {
        for (int i = 0; i < 9; ++i)
        {
            const auto angle = juce::jmap (i / 8.0f, rotaryStartAngle, rotaryEndAngle);
            const auto point = centre.getPointOnCircumference (radius - 2.0f, angle);
            g.fillEllipse (point.getX() - 3, point.getY() - 3, 6, 6);
        }
        radius -= 10.0f;
    }

    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
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

    g.setColour (outline);
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

    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                toAngle,
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::butt));
    }

    juce::Path p;
    p.startNewSubPath (centre.getPointOnCircumference (knobRadius - lineW, toAngle));
    p.lineTo (centre.getPointOnCircumference ((knobRadius - lineW) * 0.6f, toAngle));
    g.strokePath (p, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

} // namespace foleys
