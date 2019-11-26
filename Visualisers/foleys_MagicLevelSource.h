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

#pragma once

namespace foleys
{

class MagicLevelSource
{
public:

    MagicLevelSource()=default;

    /**
     Send new sample values to the measurement.
     */
    void pushSamples (const juce::AudioBuffer<float>& buffer);

    float getRMSvalue (int channel) const;
    float getMaxValue (int channel) const;

    /**
     Setup the source to measure a signal.

     @param numChannels the number of channels that will be sent
     @param sampleRate the sampleRate the signal is timed in
     @param maxKeepMS the number of milliseconds to keep the max
     @param rmsWindowMS the length to calculate the RMS of it
     */
    void setupSource (int numChannels, double sampleRate, int maxKeepMS, int rmsWindowMS);

    /**
     Set the number of channels to measure. This should be done on a non-realtime thread.
     */
    void setNumChannels (int numChannels);
    int getNumChannels() const;

    /**
     Set the number of samples to be averaged. They are stored in 64 samples blocks to minimise calculation overhead.
     */
    void setRmsLength (int numSamples);

    //==============================================================================

private:

    struct ChannelData
    {
        ChannelData()=default;
        ChannelData (const ChannelData& other);

        std::atomic<float> max;
        std::atomic<float> rms;
        std::atomic<float> overall;

        std::vector<float> rmsHistory;
        int                rmsPointer = 0;
        int                maxCountDown = 0;
    };

    std::vector<ChannelData> channelDatas;
    int rmsHistorySize = 22050;
    int maxCountDownInitial = 100;

    JUCE_DECLARE_WEAK_REFERENCEABLE (MagicLevelSource)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicLevelSource)
};

} // namespace foleys
