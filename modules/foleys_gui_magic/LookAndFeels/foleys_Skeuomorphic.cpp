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

#include "foleys_Skeuomorphic.h"

namespace foleys
{

static inline void clipToEllipse (juce::Graphics& g, juce::Rectangle<float>& bounds)
{
    juce::Path p;
    p.addEllipse (bounds);
    g.reduceClipRegion (p);
}

static inline juce::ColourGradient makeGradient (juce::Point<float> point1,
                                                 juce::Point<float> point2,
                                                 bool isRadial,
                                                 std::initializer_list<std::pair<double, juce::Colour>> list)
{
    juce::ColourGradient gr {};
    gr.isRadial = isRadial;
    gr.point1 = point1;
    gr.point2 = point2;

    for (auto pair : list)
        gr.addColour (pair.first, pair.second);

    return gr;
}

static inline void fillEllipse (juce::Graphics& g, const juce::Rectangle<float>& bounds, juce::Colour colour)
{
    g.setColour (colour);
    g.fillEllipse (bounds);
}

static inline void fillEllipse (juce::Graphics& g, const juce::Rectangle<float>& bounds, const juce::ColourGradient& gradient)
{
    g.setGradientFill (gradient);
    g.fillEllipse (bounds);
}

const Skeuomorphic::knobImages& Skeuomorphic::getKnobImages (int diameter)
{
    auto search = knobsBgs.find (diameter);
    if (search != knobsBgs.end()) return search->second;

    float radius = diameter * 0.5f;
    bool isTiny  = diameter < 20;
    bool isSmall = diameter < 60;

    juce::Point<float>     centre { radius, radius };
    juce::Rectangle<float> bounds { 0.0f, 0.0f, float (diameter), float (diameter) };

    if (knobsBgs.size() > size_t (knobsMaxSize))
        knobsBgs.clear();


    // background
    juce::Image bg { juce::Image::ARGB, diameter, diameter + 3, true };
    {
        juce::Graphics g (bg);
        fillEllipse (g, bounds.translated (0.0f, 1.0f), whiteA010);
        fillEllipse (g, bounds.translated (0.0f, 2.0f), whiteA010);
        fillEllipse (g, bounds,                         { 50, 51, 61});

        auto backgroundGr = isTiny? makeGradient (centre, { centre.x, centre.y + radius }, true,
                                               {  { 0.0, juce::Colours::transparentBlack },
                                                  { 0.9, juce::Colours::transparentBlack },
                                                  { 1.0, blackA122 }  }) :
                                    makeGradient (centre, { centre.x, centre.y + radius }, true,
                                               {  { 0.0,                          juce::Colours::transparentBlack },
                                                  { 0.52,                         blackA092 },
                                                  { 0.9,                          juce::Colours::transparentBlack },
                                                  { double (1 - (4.0f / radius)), juce::Colours::transparentBlack },
                                                  { 1.0,                          blackA092 } });
        fillEllipse (g, bounds, backgroundGr);
    }


    bounds.reduce (diameter * 0.15f, diameter * 0.15f);
    auto fgRadius = bounds.getWidth() * 0.5f;


    // foreground
    juce::Image fg { juce::Image::ARGB, diameter, diameter, true };
    {
        juce::Graphics g (fg); // lower shadow
        {
            auto xOffset = fgRadius * 0.12f;
            auto yOffset = fgRadius * 0.32f;

            juce::ColourGradient shadowGr = makeGradient ({ centre.x + xOffset, centre.y + yOffset },
                                                          { centre.x, centre.y + fgRadius + yOffset }, true,
                                                       {  { 0.0,  blackA122                 },
                                                          { 0.72, blackA122                 },
                                                          { 1.0,  juce::Colours::transparentBlack }  });
            fillEllipse (g, bounds.translated (xOffset, yOffset), shadowGr);
        }

        // middle
        {
            {
                juce::Graphics::ScopedSaveState save (g);
                clipToEllipse (g, bounds);

                juce::ColourGradient gr { { 55, 55, 66 }, 0.0f, bounds.getY(),
                                          { 34, 35, 41 }, 0.0f, bounds.getBottom(), false };
                fillEllipse (g, bounds, gr);

                juce::ColourGradient ringGr = makeGradient (centre, { centre.x, centre.y + fgRadius }, true,
                                                         {  { 0.0,  juce::Colours::transparentBlack },
                                                            { 0.72, juce::Colours::transparentBlack },
                                                            { 1.0,  blackA042                 }  });
                fillEllipse (g, bounds, ringGr);

                g.setColour (whiteA042);
                g.drawEllipse (bounds.translated (0.0f, 1.0f), 1.0f);
                if (!isSmall) g.drawEllipse (bounds.translated (0.0f, 2.0f), 1.0f);
            }

            g.setColour (blackA092);
            g.drawEllipse (bounds, 1.0f);
        }
    }

    knobsBgs.insert ({diameter, knobImages { std::move (bg), std::move (fg) }});
    return knobsBgs.find (diameter)->second;
}

void Skeuomorphic::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                     const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    int diameter = (width > height)? height : width;
    if (diameter < 16) return;

    juce::Point<float> centre (x + std::floor (width * 0.5f + 0.5f), y + std::floor (height * 0.5f + 0.5f));
    diameter -= (diameter % 2)? 9 : 8;
    float radius = diameter * 0.5f;
    x = int (centre.x - radius);
    y = int (centre.y - radius);

    const auto bounds = juce::Rectangle<int> (x, y, diameter, diameter).toFloat();

    bool isTiny  = diameter < 20;
    bool isSmall = diameter < 60;

    const auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    const juce::Colour fill = slider.findColour (juce::Slider::rotarySliderFillColourId);

    const knobImages& images = getKnobImages (diameter);
    g.drawImageAt (images.backgroundImage, x, y);

    if (!isSmall) // marker dot
    {
        auto dot = centre.getPointOnCircumference (radius * 0.825f, toAngle);
        auto dotRadius = radius * 0.072f;
        juce::Rectangle<float> dotBounds { dot.x - dotRadius, dot.y - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f };

        {
            juce::Graphics::ScopedSaveState save (g);
            fillEllipse (g, dotBounds, fill);

            clipToEllipse (g, dotBounds);

            g.setColour (fill.brighter (0.15f));
            g.drawEllipse (dotBounds.translated (0.0f, -1.0f), 1.5f);

            g.setColour (fill.darker (0.3f));
            g.drawEllipse (dotBounds.translated (0.0f, 0.5f), 1.5f);
        }

        g.setColour (blackA092);
        g.drawEllipse (dotBounds.expanded (0.5f), 1.0f);
    }

    // position arc
    {
        juce::Path valueArc;
        radius -= 1.0f;

        if (isTiny)
        {
            valueArc.addCentredArc (centre.x, centre.y, radius, radius, 0.0f, rotaryStartAngle, toAngle, true);

            g.setColour (fill.brighter (0.5f));
            g.strokePath (valueArc, juce::PathStrokeType { 3.0f });
        }
        else
        {
            juce::ColourGradient arcGr = makeGradient (centre, { centre.x, centre.y + radius }, true,
                                                    {  { 0.0,   juce::Colours::transparentWhite },
                                                       { 0.952, juce::Colours::transparentWhite },
                                                       { 1.0,   fill.withAlpha (0.8f) }      });
            g.setGradientFill (arcGr);

            valueArc.addPieSegment (bounds.reduced (1.5f), rotaryStartAngle, toAngle, 0);
            g.fillPath (valueArc);

            valueArc.clear();
            valueArc.addCentredArc (centre.x, centre.y, radius, radius, 0.0f, rotaryStartAngle, toAngle, true);

            g.setColour (fill.brighter (0.5f));
            g.strokePath (valueArc, juce::PathStrokeType { 1.5f });
        }
    }

    g.setColour (blackA092);
    g.drawEllipse (bounds, 1.0f);

    g.setColour (juce::Colours::black);
    if (!isTiny) g.drawImageAt (images.foregroundImage, x, y);
}

} // namespace foleys
