/*
 ==============================================================================
    Copyright (c) 2019-2021 Foleys Finest Audio - Daniel Walz
    All rights reserved.

    License for non-commercial projects:

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

    License for commercial products:

    To sell commercial products containing this module, you are required to buy a
    License from https://foleysfinest.com/developer/pluginguimagic/

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

#include "foleys_MagicScatterPlot.h"

namespace foleys
{

void MagicScatterPlot::pushSamples (const juce::AudioBuffer<float>& bufferIn, int currentPlotLengthIn)
{
  int numChannels = bufferIn.getNumChannels();
  int chanX = std::min<int>(0,numChannels-1);
  int chanY = std::min<int>(1,numChannels-1);
  pushSamples(/* bufferX */ bufferIn, chanX, /* bufferY */ bufferIn, chanY, currentPlotLengthIn);
}

void MagicScatterPlot::pushSamples (const juce::AudioBuffer<float>& bufferX, int channelX,
                                    const juce::AudioBuffer<float>& bufferY, int channelY,
                                    int plotLengthOverride)
{
    auto w = writePosition.load();

    plotLengthNow = std::max<int>(0,plotLengthOverride);

    const auto numSamples = bufferX.getNumSamples();
    jassert(numSamples == bufferY.getNumSamples());
    const auto available  = samplesX.getNumSamples() - w;

    const auto numChannels = bufferX.getNumChannels();
    jassert(numChannels == bufferY.getNumChannels());

    // plot (channelX,channelY):

    if (available >= numSamples)
    {
        samplesX.copyFrom (0, w, bufferX.getReadPointer (channelX), numSamples);
        samplesY.copyFrom (0, w, bufferY.getReadPointer (channelY), numSamples);
    }
    else
    {
        samplesX.copyFrom (0, w, bufferX.getReadPointer (channelX), available);
        samplesY.copyFrom (0, w, bufferY.getReadPointer (channelY), available);
        samplesX.copyFrom (0, 0, bufferX.getReadPointer (channelX, available), numSamples - available);
        samplesY.copyFrom (0, 0, bufferY.getReadPointer (channelY, available), numSamples - available);
    }

    if (available > numSamples)
        writePosition.store (w + numSamples);
    else
        writePosition.store (numSamples - available);

    resetLastDataFlag();
}

// ====================================================================================================

void MagicScatterPlot::createPlotPaths (juce::Path& path, juce::Path& filledPath, juce::Rectangle<float> bounds, MagicAudioPlotComponent&)
{
    if (sampleRate < 20.0f)
        return;

    const auto numToDisplay = getNumToDisplay(); // nominally plotLengthNow - defined in ./foleys_MagicAudioPlotSource.h
    const auto* dataX = samplesX.getReadPointer (0);
    const auto* dataY = samplesY.getReadPointer (0);

    auto position = writePosition.load() - numToDisplay;
    if (position < 0)
        position += samplesX.getNumSamples();

    if (triggeredPos || triggeredNeg) // find first zero-crossing in circular plot-buffer samplesX, giving up after 50 ms <-> 20 Hz fundamental:
    {
        auto positive = dataX [position] > 0.0f;
        auto bail = int (sampleRate / 20.0f);

        while (positive == false && --bail > 0)
        {
            if (--position < 0)
                position += samplesX.getNumSamples();

            positive = dataX [position] > 0.0f;
        }

        while (positive == true && --bail > 0)
        {
            if (--position < 0)
                position += samplesX.getNumSamples();

            positive = dataX [position] > 0.0f;
        }
    }

    // FIXME: Sum channels here if X and Y are multichannel and overlay is false

    path.clear();
    path.startNewSubPath (juce::jmap (dataX [position], -1.0f, 1.0f, bounds.getX(), bounds.getRight()),
                          juce::jmap (dataY [position], -1.0f, 1.0f, bounds.getBottom(), bounds.getY()));

    for (int i = 1; i < numToDisplay; ++i)
    {
        ++position;
        if (position >= samplesX.getNumSamples())
            position -= samplesX.getNumSamples();

        path.lineTo (juce::jmap (dataX [position], -1.0f, 1.0f, bounds.getX(), bounds.getRight()),
                     juce::jmap (dataY [position], -1.0f, 1.0f, bounds.getBottom(), bounds.getY()));
    }

    // FIXME: Make more paths here if X and Y are multichannel and overlay is true

    filledPath = path;
    filledPath.lineTo (bounds.getBottomRight());
    filledPath.lineTo (bounds.getBottomLeft());
    filledPath.closeSubPath();
}

void MagicScatterPlot::prepareToPlay (double sampleRateToUse, int)
{
    sampleRate = sampleRateToUse;

    samplesX.setSize (1, static_cast<int> (sampleRate));
    samplesX.clear();

    samplesY.setSize (1, static_cast<int> (sampleRate));
    samplesY.clear();

    writePosition.store (0);
}


} // namespace foleys
