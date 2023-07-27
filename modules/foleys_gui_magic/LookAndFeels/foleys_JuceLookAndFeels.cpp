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

#include "foleys_JuceLookAndFeels.h"

namespace foleys
{

juce::Colour JuceLookAndFeel_V2::findPopupColour (int colourId, juce::Component* target)
{
    if (target)
        return target->findColour (colourId);

    return findColour (colourId);
}

void JuceLookAndFeel_V2::drawPopupMenuItemWithOptions (juce::Graphics& g, const juce::Rectangle<int>& area,
                                                       bool isHighlighted,
                                                       const juce::PopupMenu::Item& item,
                                                       const juce::PopupMenu::Options& options)
{
    auto* target = options.getTargetComponent();

    g.fillAll (findPopupColour (isHighlighted ? juce::PopupMenu::highlightedBackgroundColourId
                                              : juce::PopupMenu::backgroundColourId,
                                target));

    const auto hasSubMenu = item.subMenu != nullptr
                            && (item.itemID == 0 || item.subMenu->getNumItems() > 0);

    if (item.isSeparator)
    {
        auto r = area.reduced (5, 0);
        r.removeFromTop (r.getHeight() / 2 - 1);

        g.setColour (juce::Colour (0x33000000));
        g.fillRect (r.removeFromTop (1));

        g.setColour (juce::Colour (0x66ffffff));
        g.fillRect (r.removeFromTop (1));
    }
    else
    {
        auto textColour = findPopupColour (juce::PopupMenu::textColourId, target);

        auto r = area.reduced (1);

        if (isHighlighted)
        {
            g.setColour (findPopupColour (juce::PopupMenu::highlightedBackgroundColourId, target));
            g.fillRect (r);

            g.setColour (findPopupColour (juce::PopupMenu::highlightedTextColourId, target));
        }
        else
        {
            g.setColour (textColour);
        }

        if (! item.isEnabled)
            g.setOpacity (0.3f);

        juce::Font font (getPopupMenuFont());

        auto maxFontHeight = (float) area.getHeight() / 1.3f;

        if (font.getHeight() > maxFontHeight)
            font.setHeight (maxFontHeight);

        g.setFont (font);

        auto iconArea = r.removeFromLeft ((r.getHeight() * 5) / 4).reduced (3).toFloat();

        auto* icon = item.image.get();
        if (icon != nullptr)
        {
            icon->drawWithin (g, iconArea,
                              juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize,
                              1.0f);
        }
        else if (item.isTicked)
        {
            auto tick = getTickShape (1.0f);
            g.fillPath (tick, tick.getTransformToScaleToFit (iconArea, true));
        }

        if (hasSubMenu)
        {
            auto arrowH = 0.6f * getPopupMenuFont().getAscent();

            auto x = (float) r.removeFromRight ((int) arrowH).getX();
            auto halfH = (float) r.getCentreY();

            juce::Path p;
            p.addTriangle (x, halfH - arrowH * 0.5f,
                           x, halfH + arrowH * 0.5f,
                           x + arrowH * 0.6f, halfH);

            g.fillPath (p);
        }

        r.removeFromRight (3);
        g.drawFittedText (item.text, r, juce::Justification::centredLeft, 1);

        if (item.shortcutKeyDescription.isNotEmpty())
        {
            juce::Font f2 (font);
            f2.setHeight (f2.getHeight() * 0.75f);
            f2.setHorizontalScale (0.95f);
            g.setFont (f2);

            g.drawText (item.shortcutKeyDescription, r, juce::Justification::centredRight, true);
        }
    }
}

//==============================================================================

juce::Colour JuceLookAndFeel_V3::findPopupColour (int colourId, juce::Component* target)
{
    if (target)
        return target->findColour (colourId);

    return findColour (colourId);
}

void JuceLookAndFeel_V3::drawPopupMenuItemWithOptions (juce::Graphics& g, const juce::Rectangle<int>& area,
                                                       bool isHighlighted,
                                                       const juce::PopupMenu::Item& item,
                                                       const juce::PopupMenu::Options& options)
{
    auto* target = options.getTargetComponent();

    g.fillAll (findPopupColour (isHighlighted ? juce::PopupMenu::highlightedBackgroundColourId
                                              : juce::PopupMenu::backgroundColourId,
                                target));

    const auto hasSubMenu = item.subMenu != nullptr
                            && (item.itemID == 0 || item.subMenu->getNumItems() > 0);

    if (item.isSeparator)
    {
        auto r = area.reduced (5, 0);
        r.removeFromTop (r.getHeight() / 2 - 1);

        g.setColour (juce::Colour (0x33000000));
        g.fillRect (r.removeFromTop (1));

        g.setColour (juce::Colour (0x66ffffff));
        g.fillRect (r.removeFromTop (1));
    }
    else
    {
        auto textColour = findPopupColour (juce::PopupMenu::textColourId, target);

        auto r = area.reduced (1);

        if (isHighlighted)
        {
            g.setColour (findPopupColour (juce::PopupMenu::highlightedBackgroundColourId, target));
            g.fillRect (r);

            g.setColour (findPopupColour (juce::PopupMenu::highlightedTextColourId, target));
        }
        else
        {
            g.setColour (textColour);
        }

        if (! item.isEnabled)
            g.setOpacity (0.3f);

        juce::Font font (getPopupMenuFont());

        auto maxFontHeight = (float) area.getHeight() / 1.3f;

        if (font.getHeight() > maxFontHeight)
            font.setHeight (maxFontHeight);

        g.setFont (font);

        auto iconArea = r.removeFromLeft ((r.getHeight() * 5) / 4).reduced (3).toFloat();

        auto* icon = item.image.get();
        if (icon != nullptr)
        {
            icon->drawWithin (g, iconArea,
                              juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize,
                              1.0f);
        }
        else if (item.isTicked)
        {
            auto tick = getTickShape (1.0f);
            g.fillPath (tick, tick.getTransformToScaleToFit (iconArea, true));
        }

        if (hasSubMenu)
        {
            auto arrowH = 0.6f * getPopupMenuFont().getAscent();

            auto x = (float) r.removeFromRight ((int) arrowH).getX();
            auto halfH = (float) r.getCentreY();

            juce::Path p;
            p.addTriangle (x, halfH - arrowH * 0.5f,
                           x, halfH + arrowH * 0.5f,
                           x + arrowH * 0.6f, halfH);

            g.fillPath (p);
        }

        r.removeFromRight (3);
        g.drawFittedText (item.text, r, juce::Justification::centredLeft, 1);

        if (item.shortcutKeyDescription.isNotEmpty())
        {
            juce::Font f2 (font);
            f2.setHeight (f2.getHeight() * 0.75f);
            f2.setHorizontalScale (0.95f);
            g.setFont (f2);

            g.drawText (item.shortcutKeyDescription, r, juce::Justification::centredRight, true);
        }
    }
}

//==============================================================================

juce::Colour JuceLookAndFeel_V4::findPopupColour (int colourId, juce::Component* target)
{
    if (target)
        return target->findColour (colourId);

    return findColour (colourId);
}

void JuceLookAndFeel_V4::drawPopupMenuItemWithOptions (juce::Graphics& g, const juce::Rectangle<int>& area,
                                                       const bool isHighlighted,
                                                       const juce::PopupMenu::Item& item,
                                                       const juce::PopupMenu::Options& options)
{
    auto textColour = findPopupColour (juce::PopupMenu::textColourId, options.getTargetComponent());

    g.fillAll (findPopupColour (isHighlighted ? juce::PopupMenu::highlightedBackgroundColourId
                                              : juce::PopupMenu::backgroundColourId,
                                options.getTargetComponent()));

    if (item.isSeparator)
    {
        auto r  = area.reduced (5, 0);
        r.removeFromTop (juce::roundToInt (((float) r.getHeight() * 0.5f) - 0.5f));
        g.setColour (textColour.withAlpha (0.3f));
        g.fillRect (r.removeFromTop (1));
    }
    else
    {
        auto r  = area.reduced (1);

        if (isHighlighted && item.isEnabled)
        {
            g.setColour (findPopupColour (juce::PopupMenu::highlightedBackgroundColourId,
                                          options.getTargetComponent()));
            g.fillRect (r);

            g.setColour (findPopupColour (juce::PopupMenu::highlightedTextColourId,
                                          options.getTargetComponent()));
        }
        else
        {
            g.setColour (textColour.withMultipliedAlpha (item.isEnabled ? 1.0f : 0.5f));
        }

        r.reduce (juce::jmin (5, area.getWidth() / 20), 0);

        auto font = getPopupMenuFont();

        auto maxFontHeight = (float) r.getHeight() / 1.3f;

        if (font.getHeight() > maxFontHeight)
            font.setHeight (maxFontHeight);

        g.setFont (font);

        auto iconArea = r.removeFromLeft (juce::roundToInt (maxFontHeight)).toFloat();

        if (item.image != nullptr)
        {
            item.image->drawWithin (g, iconArea, juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize, 1.0f);
            r.removeFromLeft (juce::roundToInt (maxFontHeight * 0.5f));
        }
        else if (item.isTicked)
        {
            auto tick = getTickShape (1.0f);
            g.fillPath (tick, tick.getTransformToScaleToFit (iconArea.reduced (iconArea.getWidth() / 5, 0).toFloat(), true));
        }

        if (item.subMenu.get() != nullptr)
        {
            auto arrowH = 0.6f * getPopupMenuFont().getAscent();

            auto x = static_cast<float> (r.removeFromRight ((int) arrowH).getX());
            auto halfH = static_cast<float> (r.getCentreY());

            juce::Path path;
            path.startNewSubPath (x, halfH - arrowH * 0.5f);
            path.lineTo (x + arrowH * 0.6f, halfH);
            path.lineTo (x, halfH + arrowH * 0.5f);

            g.strokePath (path, juce::PathStrokeType (2.0f));
        }

        r.removeFromRight (3);
        g.drawFittedText (item.text, r, juce::Justification::centredLeft, 1);

        if (item.shortcutKeyDescription.isNotEmpty())
        {
            auto f2 = font;
            f2.setHeight (f2.getHeight() * 0.75f);
            f2.setHorizontalScale (0.95f);
            g.setFont (f2);

            g.drawText (item.shortcutKeyDescription, r, juce::Justification::centredRight, true);
        }
    }
}

} // namespace foleys
