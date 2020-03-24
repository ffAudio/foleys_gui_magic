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


MagicPlotComponent::MagicPlotComponent()
{
    setColour (plotColourId, juce::Colours::orange);
    setColour (plotFillColourId, juce::Colours::orange.withAlpha (0.5f));
    setColour (plotInactiveColourId, juce::Colours::orange.darker());
    setColour (plotInactiveFillColourId, juce::Colours::orange.darker().withAlpha (0.5f));

    setOpaque (false);
}

MagicPlotComponent::~MagicPlotComponent()
{
    if (plotSource != nullptr)
        plotSource->removeChangeListener (this);
}

void MagicPlotComponent::setPlotSource (MagicPlotSource* source)
{
    if (plotSource != nullptr)
        plotSource->removeChangeListener (this);

    plotSource = source;

    if (plotSource != nullptr)
        plotSource->addChangeListener (this);
}

void MagicPlotComponent::setDecayFactor (float decayFactor)
{
    decay = decayFactor;
    updateGlowBufferSize();
}

void MagicPlotComponent::paint (juce::Graphics& g)
{
    if (plotSource == nullptr)
        return;

    if (! glowBuffer.isNull())
        drawPlotGlowing (g);
    else
        plotSource->drawPlot (g, getLocalBounds().toFloat(), *this);
}

void MagicPlotComponent::drawPlotGlowing (juce::Graphics& g)
{
    if (decay < 1.0f)
        glowBuffer.multiplyAllAlphas (decay);

    juce::Graphics glow (glowBuffer);
    plotSource->drawPlot (glow, glow.getClipBounds().toFloat(), *this);
    g.drawImageAt (glowBuffer, 0, 0);
}

void MagicPlotComponent::updateGlowBufferSize()
{
    const auto w = getWidth();
    const auto h = getHeight();

    if (decay > 0.0f && w > 0 && h > 0)
        glowBuffer = juce::Image (juce::Image::ARGB, w, h, true);
    else
        glowBuffer = juce::Image();
}

void MagicPlotComponent::resized()
{
    updateGlowBufferSize();
}

void MagicPlotComponent::changeListenerCallback (juce::ChangeBroadcaster*)
{
    triggerAsyncUpdate();
}

void MagicPlotComponent::handleAsyncUpdate()
{
    if (auto* container = findParentComponentOfClass<Container>())
        container->setChildNeedsRepaint();
}


} // namespace foleys
