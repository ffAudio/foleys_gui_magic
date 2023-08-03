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

#include <juce_graphics/juce_graphics.h>
#include <juce_audio_basics/juce_audio_basics.h>

/**
 * @file foleys_MagicAudioPlotSource.h
 * @brief API-compatible replacement for foleys_MagicPlotSource.h
 *
 * This file contains proposed extensions for MagicPlotSource
 * motivated by various audio needs.  It is in a separate file with a
 * different class name simply so that merges are never necessary
 * until adoption, if ever.
 *
 * @author Julius Smith
 * @date 2023-08-03
 * @version 0.1
 */

namespace foleys
{

class MagicAudioPlotComponent;

/**
 The MagicAudioPlotSources act as an interface, so the GUI can visualise an arbitrary plot
 of data. To create a specific new plot, create a subclass and implement drawPlot.
 */
class MagicAudioPlotSource // not worth the trouble : public MagicPlotSource
{
public:

    /** Default Constructor. */
    MagicAudioPlotSource()=default;

    /** Constructor allowing specification of a channel to display, or -1 to indicate all channels. */
    MagicAudioPlotSource(int channelToDisplay) : plotChannel(std::max<int>(0,channelToDisplay)) {}

    /** Destructor. */
    virtual ~MagicAudioPlotSource()=default;

    /**
     Set whether plot is triggered by a positive-going zero-crossing.
     @param isTriggeredPos, if true, means each plot begins at an upward zero-crossing.
            Otherwise, the latest samples received are plotted for each audio buffer.
     */
    virtual void setTriggeredPos (bool isTriggeredPos) { triggeredPos = isTriggeredPos; }

    /**
     Set whether plot is triggered by a negative-going zero-crossing.
     @param isTriggeredNeg, if true, means each plot begins at an downward zero-crossing.
            Otherwise, the latest samples received are plotted for each audio buffer.
            if both isTriggeredPos and isTriggeredNeg are true, than any zero-crossing
            will trigger the plot.
     */
    virtual void setTriggeredNeg (bool isTriggeredNeg) { triggeredNeg = isTriggeredNeg; }

    /**
     Set whether a multichannel plot is an overlay, with incrementing plot offset, or a sum of all channels.
     Normalization, if any, is applied to the final sum, not the individual channels.
     */
    virtual void setOverlay (bool overlay) { overlayPlots = overlay; }

    /**
     Set whether each channel, or the average of all channels, is renormalized to full range.
     @param isNormalizing, if true, means each audio channel, or sum of channels, is divided by
            its maximum magnitude each plot, unless the maximum falls below -80 dBFS.
     */
    virtual void setNormalize (bool isNormalizing) { normalize = isNormalizing; }

    /**
     Set whether plot is latch when it would otherwise become zero.
     @param isLatching, if true, means repeat the current plot if the next plot would be zero.
     */
    virtual void setLatch (bool isLatching) { latch = isLatching; }

    /**
     Set first audio channel to plot (numbering from 0).
     */
    virtual void setChannel (int channel) {
      plotChannel = std::max<int>(0,channel);
    }

    /**
     Set number of audio channels to plot in overlay mode, or to average if not overlaid.
     */
    virtual void setNumChannels (int nChans)
    {
        numPlotChannels = std::max<int>(0,nChans);
        if (nChans > samples.getNumChannels())
        {
            samples.setSize (nChans, static_cast<int> (sampleRate));
            samples.clear();
        }
    }

    /**
     Set default audio plot length in samples.
     */
    virtual void setPlotLength (int pl)
    {
        plotLength = std::max<int>(0,pl);
        if (plotLength > samples.getNumSamples())
            samples.setSize(samples.getNumChannels(), plotLength); // circular buffer used for plotting
    }

    /**
     Set dynamic audio plot length in samples.  This is normally set in pushSamples() for each audio buffer,
     but this function is useful for setting it back to zero to return to the default plot length.
     */
    virtual void setPlotLengthNow (int pln)
    {
        plotLengthNow = std::max<int>(0,pln);
        if (plotLengthNow > samples.getNumSamples())
            samples.setSize(samples.getNumChannels(), plotLengthNow);
        writePosition.store (0); // when plotLengthNow>0, we only write each plot from 0
    }

 public:

    /**
     Set offset between plots as a fractional value between 0 and 1 or higher, with 1 meaning adjacent nonoverlapping lanes.
     */
    virtual void setPlotOffset (float po)
    {
        plotOffset = std::max<float>(0.0f,po);
    }

    /**
     This method is called by the MagicProcessorState to allow the plot computation to be set up
     */
    virtual void prepareToPlay (double sampleRateToUse, int samplesPerBlockExpected)
    {
        sampleRate = sampleRateToUse;
        samples.setSize (1, static_cast<int> (sampleRate));
        samples.clear();
        writePosition.store (0);
    }

    /**
     This is the callback whenever new sample data arrives. It is the subclasses
     responsibility to put that into a FIFO and return as quickly as possible.
     */
    virtual void pushSamples (const juce::AudioBuffer<float>& buffer, int currentPlotLength=0)=0;
    virtual void pushSamples (const juce::AudioBuffer<float>& bufR, int channelToPlot,
                              int numChannelsToPlot, int currentPlotLength)
    {
        pushSamples(bufR,0); // FIXME: TODO
    }
    virtual void pushSamples (const std::shared_ptr<juce::AudioBuffer<float>> bufSP) { pushSamples(*bufSP.get(),0); }
    virtual void pushSamples (const std::shared_ptr<juce::AudioBuffer<float>> bufSP, int channelToPlot=0,
                              int numChannelsToPlot=1, int currentPlotLength=0)
    {
      pushSamples(*bufSP.get(),0); // FIXME: TODO
    }

    /**
     This form of the pushSamples() callback provides two channels of
     plot data, needed for XY scatterplots.

     @param bufferX is the audio buffer to serve as the X axis of the scatterplot.
     @param channelX is the audio channel number (from 0) to use for the X axis of the scatterplot.
     @param bufferY is the audio buffer to serve as the Y axis of the scatterplot.
     @param channelY is the audio channel number (from 0) to use for the Y axis of the scatterplot.
     @param currentPlotLength specifies the desired length of plots involving this audio buffer.
            Default is 0 meaning take the default (which itself defaults to 10 ms of audio data).
            A good setting for this is one period in samples, if you know what that is.
     */
    virtual void pushSamples (const juce::AudioBuffer<float>& bufferX, int channelX,
                               const juce::AudioBuffer<float>& bufferY, int channelY,
                               const int currentPlotLength=0) { }
    virtual void pushSamples (const std::shared_ptr<juce::AudioBuffer<float>> bufSPX, int channelX,
                              const std::shared_ptr<juce::AudioBuffer<float>> bufSPY, int channelY,
                              const int currentPlotLength=0) { }

    /**
     This is the callback that creates the plot for drawing.

     @param path is the path instance that is constructed by the MagicPlotSource
     @param filledPath is the path instance that is constructed by the MagicPlotSource to be filled
     @param bounds the bounds of the plot
     @param component grants access to the plot component, e.g. to find the colours from it
     */
    virtual void createPlotPaths (juce::Path& path, juce::Path& filledPath, juce::Rectangle<float> bounds, MagicAudioPlotComponent& component) = 0;

    /**
     You can add an active state to your plot to allow to paint in different colours
     */
    virtual bool isActive() const { return active; }
    virtual void setActive (bool shouldBeActive) { active = shouldBeActive; }

    /**
     Time in ms of last plot buffering.
     You can use this information to invalidate your plot drawing after some number of ms.
     */
    juce::int64 getLastDataUpdate() const { return lastData.load(); }

    /**
     Call this to reset to the current time in ms.
     */
    void resetLastDataFlag() { lastData.store (juce::Time::currentTimeMillis()); }

    /**
     If your plot needs background processing, return here a pointer to your TimeSliceClient,
     and it will automatically be added to the common background thread.
     */
    virtual juce::TimeSliceClient* getBackgroundJob() { return nullptr; }

protected:
    double                   sampleRate = 0.0;
    juce::AudioBuffer<float> samples;
    std::atomic<int>         writePosition;
    bool triggeredPos = false;
    bool triggeredNeg = false;
    bool overlayPlots = false; // true => plot channels individually (optionally offset); false => plot one sum of specified channels
    float plotOffset = 0;      // for overlays only, offset used from plot to plot
    bool normalize = false;    // normalize each plot in overlay, or final sum when not overlaying
    bool latch = false;
    int plotChannel = 0;       // first channel to plot
    int numPlotChannels = 0;   // number of channels to plot
    int plotLength = 0;        // fixed default plot length for every plot
    int plotLengthNow = 0;     // overrides plotLength when nonzero (limited to circular buffer size)

    inline void averageAllChannelsToSamplesChannel0(const juce::AudioBuffer<float>& buffer)
    {
        int w = writePosition.load();
        const auto available  = samples.getNumSamples() - w;

        const auto numSamples = buffer.getNumSamples();
        const auto numChannels = buffer.getNumChannels();
        jassert(numChannels > 0);
        const auto gain = 1.0f /  numChannels;
        int topPlotChannel = std::min<int>(numChannels, plotChannel + numPlotChannels) - 1;
        if (available >= numSamples)
        {
          // samples.copyFrom (destChannel, destStartSample, ...)
          samples.copyFrom (0, w, buffer.getReadPointer (plotChannel), numSamples, gain);
          for (int c = plotChannel+1; c <= topPlotChannel; ++c)
            samples.addFrom (0, w, buffer.getReadPointer (c), numSamples, gain);
        }
        else
        {
          samples.copyFrom (0, w, buffer.getReadPointer (plotChannel), available, gain);
          samples.copyFrom (0, 0, buffer.getReadPointer (plotChannel), numSamples - available, gain);
          for (int c = plotChannel + 1; c <= topPlotChannel; ++c)
          {
            samples.addFrom (0, w, buffer.getReadPointer (c), available, gain);
            samples.addFrom (0, 0, buffer.getReadPointer (c), numSamples - available, gain);
          }
        }
    }

    int getReadPosition(const float* data, const int pos0)
    {
        if (not triggeredPos and not triggeredNeg)
          return (pos0 >= 0 ? pos0 : pos0+samples.getNumSamples());

        int posW = pos0;
        if (posW < 0)
            posW += samples.getNumSamples();
        int posP = posW;
        int posN = posW;
        int distP = 0;
        int distN = 0;

        if (triggeredNeg) // search backward for negative-going zero-crossing
        { // in circular plot-buffer samples, giving up after 50 ms <-> 20 Hz fundamental:
            auto nonNeg = data [posN] >= 0.0f;
            auto bail = int (sampleRate / 20.0f);

            while (nonNeg == false && --bail > 0) // search back to the last negative-going zero-crossing
            {
                if (--posN < 0)
                    posN += samples.getNumSamples();
                distP++;
                nonNeg = data [posN] >= 0.0f;
            }
        }
        if (triggeredPos)
        {
            auto nonPos = data [posP] <= 0.0f;
            auto bail = samples.getNumSamples();
            while (nonPos == true && --bail > 0) // search back to the first positive-going zero-crossing
            {
                if (--posP < 0)
                    posP += samples.getNumSamples();
                distN++;
                nonPos = data [posP] >= 0.0f;
            }
            if (bail==0)
                DBG("Set samples-zero flag here and clear it in pushSamples");
        }
        int pos;
        if (triggeredPos && triggeredNeg) {
          pos = (distN > distP ? posP : posN);
        } else {
          pos = (triggeredPos ? posP : posN);
        }
        if (pos < 0)
            pos += samples.getNumSamples();
        return pos;
    }

    /* internal utility for uniformly determining current plot length */
    int getNumToDisplay() {
      if (plotLength <= 0)
          setPlotLength(int (0.01 * sampleRate));
      jassert(samples.getNumSamples()>0);
      int numToDisplay = (plotLengthNow > 0 ? std::min<int>(plotLengthNow,samples.getNumSamples()) : plotLength);
      return numToDisplay;
    }

private:
    std::atomic<juce::int64> lastData { 0 };
    bool active = true;

    JUCE_DECLARE_WEAK_REFERENCEABLE (MagicAudioPlotSource)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicAudioPlotSource)
}; // MagicAudioPlotSource

} // namespace foleys
