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

void MagicLevelSource::pushSamples (const juce::AudioBuffer<float>& buffer)
{
    for (int c=0; c < std::min (buffer.getNumChannels(), int (channelDatas.size())); ++c)
    {
        auto& data = channelDatas [c];
        data.overall.store (std::max (data.overall.load(), buffer.getMagnitude (c, 0, buffer.getNumSamples())));

        int  bufferPos = 0;
        while (bufferPos < buffer.getNumSamples())
        {
            const auto currentMax = buffer.getMagnitude (c, bufferPos, std::min (64, buffer.getNumSamples() - bufferPos));
            if (currentMax >= data.max.load() || data.maxCountDown <= 0)
            {
                data.max.store (currentMax);
                data.maxCountDown = maxCountDownInitial;
            }
            else
            {
                --data.maxCountDown;
            }

            data.rmsHistory [data.rmsPointer++] = buffer.getRMSLevel (c, bufferPos,
                                                                      std::min (64, buffer.getNumSamples() - bufferPos));
            if (data.rmsPointer >= int (data.rmsHistory.size()))
                data.rmsPointer = 0;

            bufferPos += 64;
        }

        auto sum = 0.0;
        for (const auto item : data.rmsHistory)
            sum += item * item;

        data.rms.store (static_cast<float> (std::sqrt (sum / double (data.rmsHistory.size()))));
    }
}

float MagicLevelSource::getRMSvalue (int channel) const
{
    if (juce::isPositiveAndBelow (channel, channelDatas.size()))
        return channelDatas [channel].rms.load();

    return 0.0f;
}

float MagicLevelSource::getMaxValue (int channel) const
{
    if (juce::isPositiveAndBelow (channel, channelDatas.size()))
        return channelDatas [channel].max.load();

    return 0.0f;
}

void MagicLevelSource::setupSource (int numChannels, double sampleRate, int maxKeepMS, int rmsWindowMS)
{
    setNumChannels (numChannels);
    setRmsLength (static_cast<int> (std::ceil (sampleRate * rmsWindowMS * 0.001)));

    maxCountDownInitial = static_cast<int> (std::ceil (sampleRate * maxKeepMS * 0.001 / 64.0));
}

void MagicLevelSource::setNumChannels (int numChannels)
{
    channelDatas.resize (numChannels);

    for (auto& channel : channelDatas)
        channel.rmsHistory.resize (rmsHistorySize / 64, 0.0f);
}

int MagicLevelSource::getNumChannels() const
{
    return int (channelDatas.size());
}

void MagicLevelSource::setRmsLength (int numSamples)
{
    rmsHistorySize = numSamples;

    for (auto& channel : channelDatas)
    {
        channel.rmsHistory.resize (numSamples / 64, 0.0f);
        if (channel.rmsPointer >= int (channel.rmsHistory.size()))
            channel.rmsPointer = 0;
    }
}

//==============================================================================

MagicLevelSource::ChannelData::ChannelData (const ChannelData& other)
  : max (other.max.load()),
    rms (other.rms.load()),
    overall (other.overall.load())
{
}

} // namespace foleys
