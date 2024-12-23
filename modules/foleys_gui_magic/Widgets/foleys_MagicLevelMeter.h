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

#include <juce_gui_basics/juce_gui_basics.h>

namespace foleys
{

class MagicLevelSource;

class MagicLevelMeter
  : public juce::Component
  , public juce::SettableTooltipClient
  , private juce::Timer
{
public:
    enum ColourIds
    {
        backgroundColourId = 0x2002100,
        barBackgroundColourId,
        barFillColourId,
        outlineColourId,
        tickmarkColourId
    };

    struct LookAndFeelMethods
    {
        virtual ~LookAndFeelMethods()                                                                                                       = default;
        virtual void drawMagicLevelMeter (juce::Graphics& g, MagicLevelMeter& meter, MagicLevelSource* source, juce::Rectangle<int> bounds) = 0;
    };

    MagicLevelMeter();

    void paint (juce::Graphics& g) override;

    void setLevelSource (MagicLevelSource* newSource);

    void timerCallback() override;

    void lookAndFeelChanged() override;

private:
    juce::WeakReference<MagicLevelSource> magicLevelSource;

    class LookAndFeelFallback : public LookAndFeel, public LookAndFeelMethods
    {
    public:
        LookAndFeelFallback() = default;
        void drawMagicLevelMeter (juce::Graphics& g, MagicLevelMeter& meter, MagicLevelSource* source, juce::Rectangle<int> bounds) override;
    };

    LookAndFeelFallback lookAndFeelFallback;
    LookAndFeelMethods* actualLookAndFeel = &lookAndFeelFallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicLevelMeter)
};


}  // namespace foleys
