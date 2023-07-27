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

#include "foleys_MagicLevelSource.h"

namespace foleys
{

void MagicLevelSource::pushSamples (const juce::AudioBuffer<float>& buffer)
{
    for (int c=0; c < std::min (buffer.getNumChannels(), int (channelDatas.size())); ++c)
    {
        auto& data = channelDatas [size_t (c)];
        auto currentMax = buffer.getMagnitude (c, 0, buffer.getNumSamples());
        auto currentRMS = buffer.getRMSLevel (c, 0, buffer.getNumSamples());

        data.overall.store (std::max (data.overall.load(), currentMax));

        auto lastRMS = data.rms.load();
        if (currentRMS >= lastRMS)
            data.rms.store (currentRMS);
        else
            data.rms.store (lastRMS * 0.9f); // TODO: proper decay depending on block size

        auto lastMax = data.max.load();
        if (currentMax >= lastMax)
        {
            data.max.store (currentMax);
            data.countdown = maxCountdown;
        }
        else
        {
            data.countdown -= buffer.getNumSamples();
            if (data.countdown < 0)
                data.max.store (currentMax);
        }
    }
}

float MagicLevelSource::getRMSvalue (int channel) const
{
    if (juce::isPositiveAndBelow (channel, channelDatas.size()))
        return channelDatas [size_t (channel)].rms.load();

    return 0.0f;
}

float MagicLevelSource::getMaxValue (int channel) const
{
    if (juce::isPositiveAndBelow (channel, channelDatas.size()))
        return channelDatas [size_t (channel)].max.load();

    return 0.0f;
}

void MagicLevelSource::setupSource (int numChannels, double sampleRate, int maxKeepMS)
{
    setNumChannels (numChannels);
    maxCountdown = juce::roundToInt (sampleRate * maxKeepMS / 1000);
}

void MagicLevelSource::setNumChannels (int numChannels)
{
    channelDatas.resize (size_t (numChannels));
}

int MagicLevelSource::getNumChannels() const
{
    return int (channelDatas.size());
}

//==============================================================================

MagicLevelSource::ChannelData::ChannelData (const ChannelData& other)
  : max (other.max.load()),
    rms (other.rms.load()),
    overall (other.overall.load())
{
}

} // namespace foleys
