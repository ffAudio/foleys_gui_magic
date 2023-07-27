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

#include <juce_graphics/juce_graphics.h>
#include <juce_audio_basics/juce_audio_basics.h>

namespace foleys
{

class MagicPlotComponent;

/**
 The MagicPlotSources act as an interface, so the GUI can visualise an arbitrary plot
 of data. To create a specific new plot, create a subclass and implement drawPlot.
 */
class MagicPlotSource
{
public:

    MagicPlotSource()=default;
    virtual ~MagicPlotSource()=default;

    /**
     This is the callback whenever new sample data arrives. It is the subclasses
     responsibility to put that into a FIFO and return as quickly as possible.
     */
    virtual void pushSamples (const juce::AudioBuffer<float>& buffer)=0;

    /**
     This is the callback that creates the plot for drawing.

     @param path is the path instance that is constructed by the MagicPlotSource
     @param filledPath is the path instance that is constructed by the MagicPlotSource to be filled
     @param bounds the bounds of the plot
     @param component grants access to the plot component, e.g. to find the colours from it
     */
    virtual void createPlotPaths (juce::Path& path, juce::Path& filledPath, juce::Rectangle<float> bounds, MagicPlotComponent& component) = 0;

    /**
     This method is called by the MagicProcessorState to allow the plot computation to be set up
     */
    virtual void prepareToPlay (double sampleRate, int samplesPerBlockExpected)=0;

    /**
     You can add an active state to your plot to allow to paint in different colours
     */
    virtual bool isActive() const { return active; }
    virtual void setActive (bool shouldBeActive) { active = shouldBeActive; }

    /**
     Use this information to invalidate your plot drawing
     */
    juce::int64 getLastDataUpdate() const { return lastData.load(); }

    /**
     Call this to invalidate the lastData flag
     */
    void resetLastDataFlag() { lastData.store (juce::Time::currentTimeMillis()); }

    /**
     If your plot needs background processing, return here a pointer to your TimeSliceClient,
     and it will automatically be added to the common background thread.
     */
    virtual juce::TimeSliceClient* getBackgroundJob() { return nullptr; }

private:
    std::atomic<juce::int64> lastData { 0 };
    bool active = true;

    JUCE_DECLARE_WEAK_REFERENCEABLE (MagicPlotSource)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicPlotSource)
};

} // namespace foleys
