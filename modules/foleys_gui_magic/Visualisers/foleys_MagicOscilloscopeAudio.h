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
 This class collects your samples in a circular buffer and allows the GUI to
 draw it in the style of an oscilloscope
 */
class MagicOscilloscopeAudio : public MagicAudioPlotSource
{
public:

    /**
     Create an oscilloscope adapter to push samples into for later display in the GUI.

     @param channel lets you select the channel to analyse. -1 means summing all together (the default)
     */
    MagicOscilloscopeAudio (int channelToDisplay=-1);

    static void checkAudioBufferForNaNs (juce::AudioBuffer<float>& buffer);

    /**
     Push samples of an AudioBuffer to be visualised.
     */
    void pushSamples (const juce::AudioBuffer<float>& buffer, int plotLength=0) override;
    void pushSamples (const std::shared_ptr<juce::AudioBuffer<float>> bufSP, int channelToPlot=0,
                      int numChannelsToPlot=1, int plotLength=0) override;
    void pushSamples (const juce::AudioBuffer<float>& bufR, int channelToPlot,
                      int numChannelsToPlot, int plotLength=0) override;

    /**
     This is the callback that creates the frequency plot for drawing.

      @param path is the path instance that is constructed by the MagicAudioPlotSource
      @param filledPath is the path instance that is constructed by the MagicAudioPlotSource to be filled
      @param bounds the bounds of the plot
      @param component grants access to the plot component, e.g. to find the colours from it
      */
    void createPlotPaths (juce::Path& path, juce::Path& filledPath, juce::Rectangle<float> bounds, MagicAudioPlotComponent& component) override;

    void prepareToPlay (double sampleRate, int samplesPerBlockExpected) override;

private:
    int numChannelsOut = 0; // == numChannelsIn or 1 when channels are averaged

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicOscilloscopeAudio)
};

} // namespace foleys
