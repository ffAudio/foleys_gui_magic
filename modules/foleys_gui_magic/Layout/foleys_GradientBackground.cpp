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

#include "foleys_GradientBackground.h"

namespace foleys
{

GradientBackground::GradientBackground()
{
    colours [0.0f] = juce::Colours::black;
    colours [1.0f] = juce::Colours::white;
}

void GradientBackground::setupGradientFill (juce::Graphics& g, juce::Rectangle<float> bounds)
{
    auto diag = std::sqrt (std::pow (bounds.getWidth() * std::sin (angle), 2.0f) + std::pow (bounds.getHeight() * std::cos (angle), 2.0f)) / 2.0f;
    auto vec = juce::Point<float>().getPointOnCircumference (diag, angle);

    auto p1 = type == linear ? bounds.getCentre() + vec : bounds.getCentre();
    auto p2 = bounds.getCentre() - vec;

    if (gradient.point1 != p1 || gradient.point2 != p2 || gradient.getNumColours() != int (colours.size()))
    {
        gradient.clearColours();
        gradient.point1 = p1;
        gradient.point2 = p2;
        gradient.isRadial = type == radial;
        for (auto& c : colours)
            gradient.addColour (c.first, c.second);
    }

    g.setFillType (gradient);
}

void GradientBackground::drawGradient (juce::Graphics& g, juce::Rectangle<float> bounds, const juce::Path& shape)
{
    if (isEmpty())
        return;

    juce::Graphics::ScopedSaveState state (g);
    setupGradientFill (g, bounds);

    g.fillPath (shape);
}

void GradientBackground::setup (juce::String text, const Stylesheet& stylesheet)
{
    clear();

    if (text.isEmpty())
    {
        colours [0.0f] = juce::Colours::black;
        colours [1.0f] = juce::Colours::white;
        return;
    }

    if (text.startsWith ("linear"))
        type = linear;
    else if (text.startsWith ("radial"))
        type = radial;

    text = text.fromFirstOccurrenceOf ("(", false, false).upToLastOccurrenceOf (")", false, false);

    auto values = juce::StringArray::fromTokens (text, ",;", "\"");
    if (values.size() < 2)
        return;

    if (type == linear)
    {
        angle = juce::degreesToRadians (values [0].getFloatValue());
        values.remove (0);
    }

    auto step = 1.0f / (values.size() - 1.0f);
    auto stop = 0.0f;
    for (const auto& v : values)
    {
        auto items = juce::StringArray::fromTokens (v, " ", "\"");
        if (items.size() > 1)
            stop = items [0].getFloatValue() / 100.0f;

        colours [stop] = stylesheet.getColour (items.strings.getLast());
        stop += step;
    }
}

juce::String GradientBackground::toString() const
{
    juce::String colourNames;

    if (type == linear)
        colourNames += juce::String (juce::roundToInt (juce::radiansToDegrees (angle))) + ",";

    for (auto& c : colours)
        colourNames += juce::String (juce::roundToInt (c.first * 100.0f)) + "% " + c.second.toString() + ",";

    colourNames = colourNames.trimCharactersAtEnd (", ");

    if (type == linear)
        return "linear-gradient(" + colourNames + ")";
    else if (type == radial)
        return "radial-gradient(" + colourNames + ")";
    else
        return "none(" + colourNames + ")";
}

void GradientBackground::clear()
{
    type = none;
    colours.clear();
    gradient = juce::ColourGradient();
}

bool GradientBackground::isEmpty() const
{
    return type == none || colours.size() < 2;
}

} // namespace foleys
