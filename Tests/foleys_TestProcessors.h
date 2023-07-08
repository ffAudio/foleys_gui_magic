/*
 ==============================================================================
    Copyright (c) 2022 Foleys Finest Audio - Daniel Walz
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

#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>

class UnitTestProcessor : public foleys::MagicProcessor
{
public:
    UnitTestProcessor()
    {
        addParameter (new juce::AudioParameterFloat ("float", "Some float", {-100.0f, 100.0f}, 10.0f));
        addParameter (new juce::AudioParameterBool ("bool", "Some boolean", false));
        addParameter (new juce::AudioParameterChoice ("choice", "Choose wisely", {"Gold", "Tin", "Wood", "Marble"}, 2));
        addParameter (new juce::AudioParameterInt ("int", "Some number", -10, 10, 3));
    }

    const juce::String getName() const override { return "UnitTestProcessor"; }
    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override { juce::ignoreUnused (sampleRate, maximumExpectedSamplesPerBlock); }
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override { juce::ignoreUnused (buffer, midi); }
    void releaseResources() override {}

    double getTailLengthSeconds() const override { return 0; }
    bool acceptsMidi() const override            { return true; }
    bool producesMidi() const override           { return false; }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnitTestProcessor)
};


