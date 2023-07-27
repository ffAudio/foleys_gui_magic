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

class MidiDrumpadComponent : public juce::Component,
                             public juce::Timer
{
public:
    enum ColourIds
    {
        background= 0x2002200,
        padFill,
        padOutline,
        padDownFill,
        padDownOutline,
        touch
    };

    MidiDrumpadComponent (juce::MidiKeyboardState& keyboardState);
    ~MidiDrumpadComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    /**
     Set the number of rows and columns. The note numbers are ascending top left to bottom right.
     */
    void setMatrix (int rows, int columns);

    /**
     Set the note number of the top left pad
     */
    void setRootNote (int noteNumber);

    void timerCallback() override;

    class Pad : public juce::Component,
                public juce::MidiKeyboardState::Listener
    {
    public:
        Pad (MidiDrumpadComponent& owner, int noteNumber);
        ~Pad() override;

        void paint (juce::Graphics& g) override;

        void mouseDown (const juce::MouseEvent& event) override;
        void mouseDrag (const juce::MouseEvent& event) override;
        void mouseUp (const juce::MouseEvent& event) override;

        void handleNoteOn (juce::MidiKeyboardState* source,
                           int midiChannel, int midiNoteNumber, float velocity) override;

        void handleNoteOff (juce::MidiKeyboardState* source,
                            int midiChannel, int midiNoteNumber, float velocity) override;

    private:
        MidiDrumpadComponent& owner;
        int                   noteNumber = 60;
        std::atomic_bool      isDown { false };
        juce::Point<int>      lastPos;
        float                 pressure = 0.0f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pad)
    };

private:
    void updateButtons();

    juce::MidiKeyboardState& keyboardState;

    int rootNote   = 60;  // C3
    int numRows    =  3;
    int numColumns =  3;

    std::atomic_bool                  needsPaint { true };
    std::vector<std::unique_ptr<Pad>> pads;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiDrumpadComponent)
};


} // namespace foleys
