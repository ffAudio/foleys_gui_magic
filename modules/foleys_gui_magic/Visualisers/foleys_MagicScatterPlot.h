/*
 ==============================================================================
    Copyright (c) 2023 Julius Smith and Foleys Finest Audio - Daniel Walz
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

#pragma once

namespace foleys
{

class MagicAudioPlotComponent;

/**
 This class collects two buffers of samples in a circular buffer and
 allows the GUI to draw them in the style of an scatterplot, or
 XY-plot.  For example, sin(t) and cos(t) produce a circle.
 */
class MagicScatterPlot : public MagicAudioPlotSource
{
public:

    /**
     Create an XY ScatterPlot adapter to push samples into for later display in the GUI.
     */
    MagicScatterPlot () : MagicAudioPlotSource() {}

    /**
     Push samples to a buffer to be visualised as a scatterplot (XY plot) of channels 0 (X) and 1 (Y).
     */
    void pushSamples (const juce::AudioBuffer<float>& buffer, int currentPlotLength) override;

    /**
     Push samples to a buffer to be visualised as a scatterplot (XY plot).

      @param bufferX is plotted as the X-axis coordinate.
      @param bufferY is plotted as the Y-axis coordinate.
      @param plotLength, if positive, gives the preferred length of
             the next plot in samples (e.g., one period).
             Otherwise, 10 ms of samples is plotted.
     */
    void pushSamples (const juce::AudioBuffer<float>& bufferX, int channelX,
                      const juce::AudioBuffer<float>& bufferY, int channelY,
                      const int plotLengthOverride=0) override;

    /**
     This is the callback that creates the frequency plot for drawing.

      @param path is the path instance that is constructed by the MagicPlotSource
      @param filledPath is the path instance that is constructed by the MagicPlotSource to be filled
      @param bounds the bounds of the plot
      @param component grants access to the plot component, e.g. to find the colours from it
      */
    virtual void createPlotPaths (juce::Path& path, juce::Path& filledPath, juce::Rectangle<float> bounds, MagicAudioPlotComponent& component) override;

    virtual void prepareToPlay (double sampleRate, int samplesPerBlockExpected) override;

private:

    juce::AudioBuffer<float> samplesX;
    juce::AudioBuffer<float> samplesY;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicScatterPlot)
};

} // namespace foleys
