/*
 ==============================================================================
    Copyright (c) 2019-2023 Foleys Finest Audio - Daniel Walz
    All rights reserved.

    **BSD 3-Clause License**

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

 ==============================================================================

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

#include "foleys_LookAndFeel.h"

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

    if (slider.isEnabled() && arcRadius > 10.0f)
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

//==============================================================================

void LookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool,
                                int, int, int, int, juce::ComboBox& box)
{
    auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    juce::Rectangle<int> boxBounds (0, 0, width, height);

    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

    g.setColour (box.findColour (juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle (boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);
}

void LookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    label.setBounds (1, 1,
                     box.getWidth() - 2,
                     box.getHeight() - 2);

    label.setFont (getComboBoxFont (box));
}

//==============================================================================

void LookAndFeel::drawTabButton (juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown)
{
    const auto activeArea = button.getActiveArea();

    const auto o = button.getTabbedButtonBar().getOrientation();

    const auto bkg = button.getTabBackgroundColour();

    juce::Point<int> p1, p2;

    switch (o)
    {
        case juce::TabbedButtonBar::TabsAtBottom:   p1 = activeArea.getBottomLeft(); p2 = activeArea.getTopLeft();    break;
        case juce::TabbedButtonBar::TabsAtTop:      p1 = activeArea.getTopLeft();    p2 = activeArea.getBottomLeft(); break;
        case juce::TabbedButtonBar::TabsAtRight:    p1 = activeArea.getTopRight();   p2 = activeArea.getTopLeft();    break;
        case juce::TabbedButtonBar::TabsAtLeft:     p1 = activeArea.getTopLeft();    p2 = activeArea.getTopRight();   break;
        default:                              jassertfalse; break;
    }

    if (button.getToggleState())
    {
        g.setGradientFill (juce::ColourGradient (bkg.brighter (0.1f), p1.toFloat(),
                                                 bkg.darker   (0.2f), p2.toFloat(), false));
    }
    else
    {
        g.setGradientFill (juce::ColourGradient (bkg.darker (0.2f), p1.toFloat(),
                                                 bkg.darker (0.5f), p2.toFloat(), false));
    }

    g.fillRect (activeArea);

    g.setColour (button.findColour (juce::TabbedButtonBar::tabOutlineColourId));

    auto r = activeArea;

    if (o != juce::TabbedButtonBar::TabsAtBottom)   g.fillRect (r.removeFromTop (1));
    if (o != juce::TabbedButtonBar::TabsAtTop)      g.fillRect (r.removeFromBottom (1));
    if (o != juce::TabbedButtonBar::TabsAtRight)    g.fillRect (r.removeFromLeft (1));
    if (o != juce::TabbedButtonBar::TabsAtLeft)     g.fillRect (r.removeFromRight (1));

    const auto alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;

    auto col = bkg.contrasting().withMultipliedAlpha (alpha);

    if (juce::TabbedButtonBar* bar = button.findParentComponentOfClass<juce::TabbedButtonBar>())
    {
        juce::TabbedButtonBar::ColourIds colID = button.isFrontTab() ? juce::TabbedButtonBar::frontTextColourId
                                                                     : juce::TabbedButtonBar::tabTextColourId;

        if (bar->isColourSpecified (colID))
            col = bar->findColour (colID);
        else if (isColourSpecified (colID))
            col = findColour (colID);
    }

    const auto area = button.getTextArea().toFloat();

    float length = area.getWidth();
    float depth  = area.getHeight();

    if (button.getTabbedButtonBar().isVertical())
        std::swap (length, depth);

    juce::TextLayout textLayout;
    createTabTextLayout (button, length, depth, col, textLayout);

    juce::AffineTransform t;

    switch (o)
    {
        case juce::TabbedButtonBar::TabsAtLeft:   t = t.rotated (juce::MathConstants<float>::pi * -0.5f).translated (area.getX(), area.getBottom()); break;
        case juce::TabbedButtonBar::TabsAtRight:  t = t.rotated (juce::MathConstants<float>::pi *  0.5f).translated (area.getRight(), area.getY()); break;
        case juce::TabbedButtonBar::TabsAtTop:
        case juce::TabbedButtonBar::TabsAtBottom: t = t.translated (area.getX(), area.getY()); break;
        default:                                  jassertfalse; break;
    }

    g.addTransform (t);
    textLayout.draw (g, juce::Rectangle<float> (length, depth));
}

} // namespace foleys
