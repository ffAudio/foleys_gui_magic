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

#pragma once

namespace foleys
{

class MidiDrumpadComponent : public juce::Component
{
public:
    MidiDrumpadComponent (juce::MidiKeyboardState& keyboardState);

    void paint (juce::Graphics& g) override;
    void resized() override;

    void setMatrix (int rows, int columns);
    void setRootNote (int noteNumber);

    class Pad : public juce::Component
    {
    public:
        Pad (juce::MidiKeyboardState& keyboardState, int noteNumber);

        void paint (juce::Graphics& g) override;

        void mouseDown (const juce::MouseEvent& event) override;
        void mouseUp (const juce::MouseEvent& event) override;

    private:
        juce::MidiKeyboardState& keyboardState;
        int noteNumber = 64;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pad)
    };

private:
    void updateButtons();

    juce::MidiKeyboardState& keyboardState;

    int rootNote   = 64;
    int numRows    =  3;
    int numColumns =  3;

    std::vector<std::unique_ptr<Pad>> pads;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiDrumpadComponent)
};


} // namespace foleys
