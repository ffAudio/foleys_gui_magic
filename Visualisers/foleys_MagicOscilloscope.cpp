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


MagicOscilloscope::MagicOscilloscope (int channelToDisplay)
  : channel (channelToDisplay)
{
}

void MagicOscilloscope::pushSamples (const juce::AudioBuffer<float>& buffer)
{
    auto w = writePosition.load();
    const auto numSamples = buffer.getNumSamples();
    const auto available  = samples.getNumSamples() - w;

    if (channel < 0)
    {
        // mono summing all channels and average
        const auto gain = 1.0f / buffer.getNumChannels();
        if (available >= numSamples)
        {
            samples.copyFrom (0, w, buffer.getReadPointer (0), numSamples, gain);
            for (int c = 1; c < buffer.getNumChannels(); ++c)
                samples.addFrom (0, w, buffer.getReadPointer (c), numSamples, gain);
        }
        else
        {
            samples.copyFrom (0, w, buffer.getReadPointer (0),            available, gain);
            samples.copyFrom (0, 0, buffer.getReadPointer (0, available), numSamples - available, gain);
            for (int c = 1; c < buffer.getNumChannels(); ++c)
            {
                samples.addFrom (0, w, buffer.getReadPointer (c),            available, gain);
                samples.addFrom (0, 0, buffer.getReadPointer (c, available), numSamples - available, gain);
            }
        }
    }
    else
    {
        // plotting individual channel
        if (available >= numSamples)
        {
            samples.copyFrom (0, w, buffer.getReadPointer (channel), numSamples);
        }
        else
        {
            samples.copyFrom (0, w, buffer.getReadPointer (channel),            available);
            samples.copyFrom (0, 0, buffer.getReadPointer (channel, available), numSamples - available);
        }
    }

    if (available > numSamples)
        writePosition.store (w + numSamples);
    else
        writePosition.store (numSamples - available);

    resetLastDataFlag();
}

void MagicOscilloscope::createPlotPaths (juce::Path& path, juce::Path& filledPath, juce::Rectangle<float> bounds, MagicPlotComponent&)
{
    if (sampleRate < 20.0f)
        return;

    const auto  numToDisplay = int (0.01 * sampleRate) - 1;
    const auto* data = samples.getReadPointer (0);

    auto pos = writePosition.load() - numToDisplay;
    if (pos < 0)
        pos += samples.getNumSamples();

    // trigger
    auto sign = data [pos] > 0.0f;
    auto bail = int (sampleRate / 20.0f);

    while (sign == false && --bail > 0)
    {
        if (--pos < 0)
            pos += samples.getNumSamples();

        sign = data [pos] > 0.0f;
    }

    while (sign == true && --bail > 0)
    {
        if (--pos < 0)
            pos += samples.getNumSamples();

        sign = data [pos] > 0.0f;
    }

    path.clear();
    path.startNewSubPath (bounds.getX(),
                          juce::jmap (data [pos], -1.0f, 1.0f, bounds.getBottom(), bounds.getY()));

    for (int i = 1; i < numToDisplay; ++i)
    {
        ++pos;
        if (pos >= samples.getNumSamples())
            pos -= samples.getNumSamples();

        path.lineTo (juce::jmap (float (i),   0.0f, float (numToDisplay), bounds.getX(), bounds.getRight()),
                     juce::jmap (data [pos], -1.0f, 1.0f,                 bounds.getBottom(), bounds.getY()));
    }

    filledPath = path;
    filledPath.lineTo (bounds.getBottomRight());
    filledPath.lineTo (bounds.getBottomLeft());
    filledPath.closeSubPath();
}

void MagicOscilloscope::prepareToPlay (double sampleRateToUse, int)
{
    sampleRate = sampleRateToUse;

    samples.setSize (1, static_cast<int> (sampleRate));
    samples.clear();

    writePosition.store (0);
}


} // namespace foleys
