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

#include "foleys_MagicLevelMeter.h"
#include "../Visualisers/foleys_MagicLevelSource.h"

namespace foleys
{

MagicLevelMeter::MagicLevelMeter()
{
    setColour (backgroundColourId, juce::Colours::transparentBlack);
    setColour (barBackgroundColourId, juce::Colours::darkgrey);
    setColour (barFillColourId, juce::Colours::darkgreen);
    setColour (outlineColourId, juce::Colours::silver);
    setColour (tickmarkColourId, juce::Colours::silver);

    startTimerHz (30);
}

void MagicLevelMeter::paint (juce::Graphics& g)
{
    if (auto* lnf = dynamic_cast<LookAndFeelMethods*>(&getLookAndFeel()))
    {
        lnf->drawLevelMeter (g, *this, source, getLocalBounds());
        return;
    }

    const auto backgroundColour = findColour (backgroundColourId);
    if (!backgroundColour.isTransparent())
        g.fillAll (backgroundColour);

    if (source == nullptr)
        return;

    const auto numChannels = source->getNumChannels();
    if (numChannels == 0)
        return;

    auto bounds = getLocalBounds().reduced (3).toFloat();

    const auto width = bounds.getWidth() / numChannels;
    const auto barBackgroundColour = findColour (barBackgroundColourId);
    const auto barFillColour = findColour (barFillColourId);
    const auto outlineColour = findColour (outlineColourId);

    const auto infinity = -100.0f;
    for (int i=0; i < numChannels; ++i)
    {
        auto bar = bounds.removeFromLeft (width).reduced (1);
        g.setColour (barBackgroundColour);
        g.fillRect (bar);
        g.setColour (outlineColour);
        g.drawRect (bar, 1.0f);
        bar.reduce (1, 1);
        g.setColour (barFillColour);
        g.fillRect (bar.withTop (juce::jmap (juce::Decibels::gainToDecibels (source->getRMSvalue (i), infinity),
                                             infinity, 0.0f, bar.getBottom(), bar.getY())));
        g.drawHorizontalLine (juce::roundToInt (juce::jmap (juce::Decibels::gainToDecibels (source->getMaxValue (i), infinity),
                                                            infinity, 0.0f, bar.getBottom (), bar.getY ())),
                              static_cast<float>(bar.getX ()), static_cast<float>(bar.getRight ()));
    }
}

void MagicLevelMeter::setLevelSource (MagicLevelSource* newSource)
{
    source = newSource;
}

void MagicLevelMeter::timerCallback()
{
    repaint();
}

} // namespace foleys
