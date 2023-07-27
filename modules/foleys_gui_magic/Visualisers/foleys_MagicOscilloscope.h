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

#pragma once

#include "foleys_MagicPlotSource.h"

namespace foleys
{

/**
 This class collects your samples in a circular buffer and allows the GUI to
 draw it in the style of an oscilloscope
 */
class MagicOscilloscope : public MagicPlotSource
{
public:

    /**
     Create an oscilloscope adapter to push samples into for later display in the GUI.

     @param channel lets you select the channel to analyse. -1 means summing all together (the default)
     */
    MagicOscilloscope (int channel=-1);

    /**
     Push samples to a buffer to be visualised.
     */
    void pushSamples (const juce::AudioBuffer<float>& buffer) override;

    /**
     This is the callback that creates the frequency plot for drawing.

      @param path is the path instance that is constructed by the MagicPlotSource
      @param filledPath is the path instance that is constructed by the MagicPlotSource to be filled
      @param bounds the bounds of the plot
      @param component grants access to the plot component, e.g. to find the colours from it
      */
    void createPlotPaths (juce::Path& path, juce::Path& filledPath, juce::Rectangle<float> bounds, MagicPlotComponent& component) override;

    void prepareToPlay (double sampleRate, int samplesPerBlockExpected) override;

private:
    int                      channel = -1;
    double                   sampleRate = 0.0;

    juce::AudioBuffer<float> samples;
    std::atomic<int>         writePosition;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicOscilloscope)
};

} // namespace foleys
