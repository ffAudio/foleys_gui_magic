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

/**
 This will plot the magnitudes of the frequencies in a signal. The processing happens in a worker thread
 to keep the audio thread free.
 */
class MagicAnalyser : public MagicPlotSource
{
public:

    /**
     Creates a MagicAnalyser, that will calculate a frequency plot (FFT) each time new samples occur.

     @param channel lets you select the channel to analyse. -1 means summing all together (the default)
     */
    MagicAnalyser (int channel=-1);

    /**
     Push new samples to the buffer, so a background worker can create a frequency plot
     of it.

     @param buffer the buffer of new audio
     */
    void pushSamples (const juce::AudioBuffer<float>& buffer) override;

    /**
     This is the callback that creates the plot for drawing.

     @param path is the path instance that is constructed by the MagicPlotSource
     @param filledPath is the path instance that is constructed by the MagicPlotSource to be filled
     @param bounds the bounds of the plot
     @param component grants access to the plot component, e.g. to find the colours from it
     */
    void createPlotPaths (juce::Path& path, juce::Path& filledPath, juce::Rectangle<float> bounds, MagicPlotComponent& component) override;

    /**
     This method is called by the MagicProcessorState to allow the plot computation to be set up
     */
    void prepareToPlay (double sampleRate, int samplesPerBlockExpected) override;

    /**
     If your plot needs background processing, return here a pointer to your TimeSliceClient,
     and it will automatically be added to the common background thread.
     */
    juce::TimeSliceClient* getBackgroundJob() override;

private:

    float indexToX (int index, float minFreq) const;
    float binToY (float bin, const juce::Rectangle<float> bounds) const;

    class AnalyserJob : public juce::TimeSliceClient
    {
    public:
        AnalyserJob (MagicAnalyser& owner);
        int useTimeSlice() override;

        void pushSamples (const juce::AudioBuffer<float>& buffer, int channel);

        void setupAnalyser (int audioFifoSize);

        const juce::AudioBuffer<float> getAnalyserData() const;

        juce::dsp::FFT fft                            { 12 };

    private:
        MagicAnalyser& owner;

        juce::AbstractFifo abstractFifo               { 48000 };
        juce::AudioBuffer<float> audioFifo;

        juce::dsp::WindowingFunction<float> windowing { size_t (fft.getSize()), juce::dsp::WindowingFunction<float>::hann, true };
        juce::AudioBuffer<float> fftBuffer            { 1, fft.getSize() * 2 };

        juce::AudioBuffer<float> averager             { 5, fft.getSize() / 2 };
        int averagerPtr = 1;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalyserJob)
    };

    double            sampleRate {};

    int               channel = -1;

    juce::CriticalSection pathCreationLock;
    AnalyserJob analyserJob { *this };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicAnalyser)
};

} // namespace foleys
