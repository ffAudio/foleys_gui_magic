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

#pragma once

#include "foleys_BoxModel.h"
#include "foleys_GradientBackground.h"

namespace foleys
{

class MagicGUIBuilder;

class Decorator
{
public:

    Decorator() = default;

    /**
     This will get the necessary information from the stylesheet, using inheritance
     of nodes if needed, to set the margins/borders etc. for the GuiItem.
     */
    void configure (MagicGUIBuilder& builder, const juce::ValueTree& node);

    void reset();

    void updateColours (MagicGUIBuilder& builder, const juce::ValueTree& node);

    void drawDecorator (juce::Graphics& g, juce::Rectangle<int> bounds);

    struct ClientBounds
    {
        juce::Rectangle<int> client;
        juce::Rectangle<int> caption;
    };

    Decorator::ClientBounds getClientBounds (juce::Rectangle<int> overallBounds) const;

    juce::String getTabCaption (const juce::String& defaultName) const;
    juce::Colour getTabColour() const;

    juce::Colour getBackgroundColour() const;

private:

    juce::Colour backgroundColour { juce::Colours::darkgrey };
    juce::Colour borderColour     { juce::Colours::silver };

    Box<float> margin  { 5.0f };
    Box<float> padding { 5.0f };
    float border  = 0.0f;
    float radius  = 5.0f;

    juce::String        caption;
    juce::Justification justification = juce::Justification::centredTop;
    float               captionSize   = 20.0f;
    juce::Colour        captionColour = juce::Colours::silver;

    juce::String        tabCaption;
    juce::Colour        tabColour;

    juce::Image                 backgroundImage;
    float                       backgroundAlpha = 1.0f;
    juce::RectanglePlacement    backgroundPlacement = juce::RectanglePlacement::centred;
    GradientBackground          backgroundGradient;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Decorator)
};

}
