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

#include "foleys_Decorator.h"

namespace foleys
{

void Decorator::drawDecorator (juce::Graphics& g, juce::Rectangle<int> bounds)
{
    juce::Graphics::ScopedSaveState stateSave (g);

    auto boundsf = margin.reducedRect (bounds.toFloat());

    {
        juce::Graphics::ScopedSaveState save (g);

        if (! backgroundGradient.isEmpty())
        {
            juce::Path p;
            p.addRoundedRectangle (boundsf, radius);
            backgroundGradient.drawGradient (g, boundsf, p);
        }
        else
        {
            g.setColour (backgroundColour);

            if (radius > 0.0f)
                g.fillRoundedRectangle (boundsf, radius);
            else
                g.fillRect (bounds);
        }
    }

    if (! backgroundImage.isNull())
    {
        juce::Graphics::ScopedSaveState save (g);
        g.setOpacity (backgroundAlpha);
        g.drawImage (backgroundImage, boundsf, backgroundPlacement);
    }

    if (border > 0.0f)
    {
        g.setColour (borderColour);

        if (radius > 0.0f)
            g.drawRoundedRectangle (boundsf, radius, border);
        else
            g.drawRect (bounds, juce::roundToInt (border));
    }

    if (caption.isNotEmpty())
    {
        auto clientBounds = getClientBounds (bounds);

        g.setColour (captionColour);

        g.setFont (captionSize * 0.8f);
        g.drawFittedText (caption, clientBounds.caption, justification.getOnlyHorizontalFlags(), 1);
    }
}

juce::String Decorator::getTabCaption (const juce::String& defaultName) const
{
    return tabCaption.isNotEmpty() ? tabCaption
                                   : caption.isNotEmpty() ? caption
                                                          : defaultName;
}

juce::Colour Decorator::getTabColour() const
{
    return tabColour;
}

juce::Colour Decorator::getBackgroundColour() const
{
    return backgroundColour;
}

void Decorator::updateColours (MagicGUIBuilder& builder, const juce::ValueTree& node)
{
    auto& stylesheet = builder.getStylesheet();

    auto bg = builder.getStyleProperty (IDs::backgroundColour, node);
    if (! bg.isVoid())
        backgroundColour = stylesheet.getColour (bg.toString());

    auto bcVar = builder.getStyleProperty (IDs::borderColour, node);
    if (! bcVar.isVoid())
        borderColour = stylesheet.getColour (bcVar.toString());

    auto ccVar = builder.getStyleProperty (IDs::captionColour, node);
    if (! ccVar.isVoid())
        captionColour = stylesheet.getColour (ccVar.toString());
}

Decorator::ClientBounds Decorator::getClientBounds (juce::Rectangle<int> overallBounds) const
{
    auto box = padding.reducedRect (margin.reducedRect (overallBounds.toFloat()));
    juce::Rectangle<int> captionBox;

    if (caption.isNotEmpty())
    {
        if (justification == juce::Justification::centred)
            captionBox = overallBounds;
        else if (justification.getOnlyVerticalFlags() & juce::Justification::top)
            captionBox = box.removeFromTop (captionSize).toNearestInt();
        else if (justification.getOnlyVerticalFlags() & juce::Justification::bottom)
            captionBox = box.removeFromBottom (captionSize).toNearestInt();
        else
        {
            juce::Font f (captionSize * 0.8f);
            auto w = float (f.getStringWidth (caption));

            if (justification.getOnlyHorizontalFlags() & juce::Justification::left)
                captionBox = box.removeFromLeft (w).toNearestInt();
            else if (justification.getOnlyHorizontalFlags() & juce::Justification::right)
                captionBox = box.removeFromRight (w).toNearestInt();
        }
    }

    return { box.toNearestInt(), captionBox };
}

void Decorator::configure (MagicGUIBuilder& builder, const juce::ValueTree& node)
{
    auto& stylesheet = builder.getStylesheet();

    auto borderVar = builder.getStyleProperty (IDs::border, node);
    if (! borderVar.isVoid())
        border = static_cast<float> (borderVar);

    auto marginVar = builder.getStyleProperty (IDs::margin, node);
    if (! marginVar.isVoid())
        margin = Box<float>::fromString (marginVar.toString());

    auto paddingVar = builder.getStyleProperty (IDs::padding, node);
    if (! paddingVar.isVoid())
        padding = Box<float>::fromString (paddingVar.toString());

    auto radiusVar = builder.getStyleProperty (IDs::radius, node);
    if (! radiusVar.isVoid())
        radius = static_cast<float> (radiusVar);

    caption    = builder.getStyleProperty (IDs::caption, node);
    tabCaption = builder.getStyleProperty (IDs::tabCaption, node);
    auto tc    = builder.getStyleProperty (IDs::tabColour, node);
    if (! tc.isVoid())
        tabColour = stylesheet.getColour (tc.toString());

    auto sizeVar = builder.getStyleProperty (IDs::captionSize, node);
    if (! sizeVar.isVoid())
        captionSize = static_cast<float> (sizeVar);

    auto placementVar = builder.getStyleProperty (IDs::captionPlacement, node);
    if (! placementVar.isVoid() && placementVar.toString().isNotEmpty())
        justification = juce::Justification (makeJustificationsChoices()[placementVar.toString()]);
    else
        justification = juce::Justification::centredTop;

    backgroundImage = stylesheet.getBackgroundImage (node);
    backgroundGradient.setup (builder.getStyleProperty (IDs::backgroundGradient, node).toString(), stylesheet);

    auto alphaVar = builder.getStyleProperty (IDs::backgroundAlpha, node);
    if (! alphaVar.isVoid())
        backgroundAlpha = static_cast<float> (alphaVar);

    auto backPlacement = builder.getStyleProperty (IDs::imagePlacement, node);
    if (! backPlacement.isVoid())
    {
        if (backPlacement.toString() == IDs::imageStretch)
            backgroundPlacement = juce::RectanglePlacement::stretchToFit;
        else if (backPlacement.toString() == IDs::imageFill)
            backgroundPlacement = juce::RectanglePlacement::fillDestination;
        else if (backPlacement.toString() == IDs::imageCentred)
            backgroundPlacement = juce::RectanglePlacement::centred;
    }
}

void Decorator::reset()
{
    backgroundColour = juce::Colours::darkgrey;
    borderColour     = juce::Colours::silver;

    margin  = { 5.0f };
    padding = { 5.0f };
    border  = 0.0f;
    radius  = 5.0f;

    caption.clear();
    justification = juce::Justification::centredTop;
    captionSize   = 20.0f;
    captionColour = juce::Colours::silver;

    tabCaption.clear();
    tabColour = juce::Colours::darkgrey;

    backgroundImage = juce::Image();
    backgroundAlpha = 1.0f;
    backgroundPlacement = juce::RectanglePlacement::centred;
    backgroundGradient.clear();
}

}
