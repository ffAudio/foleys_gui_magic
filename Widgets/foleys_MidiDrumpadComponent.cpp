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


namespace foleys
{

MidiDrumpadComponent::MidiDrumpadComponent (juce::MidiKeyboardState& state)
  : keyboardState (state)
{
    updateButtons();
}

void MidiDrumpadComponent::setMatrix (int rows, int columns)
{
    numRows = rows;
    numColumns = columns;
    
    updateButtons();
}

void MidiDrumpadComponent::setRootNote (int noteNumber)
{
    rootNote = noteNumber;
    updateButtons();
}

void MidiDrumpadComponent::updateButtons()
{
    pads.clear();

    for (int i = 0; i < numRows * numColumns; ++i)
        pads.push_back (std::make_unique<Pad>(keyboardState, rootNote + i));

    for (auto& pad : pads)
        addAndMakeVisible (pad.get());

    resized();
}

void MidiDrumpadComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
}

void MidiDrumpadComponent::resized()
{
    auto w = getWidth() / numColumns;
    auto h = getHeight() / numRows;

    int i = 0;
    for (auto& pad : pads)
    {
        pad->setBounds (w * (i % numColumns), h * (i / numColumns), w, h);
        ++i;
    }
}

//  ==============================================================================

MidiDrumpadComponent::Pad::Pad (juce::MidiKeyboardState& state, int note)
  : keyboardState (state),
    noteNumber (note)
{}

void MidiDrumpadComponent::Pad::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::green);
    g.fillRoundedRectangle (getLocalBounds().reduced (3).toFloat(), 3.0);
}

void MidiDrumpadComponent::Pad::mouseDown (const juce::MouseEvent& event)
{
    keyboardState.noteOn (1, noteNumber, event.isPressureValid() ? event.pressure : 1.0f);
}

void MidiDrumpadComponent::Pad::mouseUp (const juce::MouseEvent& event)
{
    juce::ignoreUnused (event);
    keyboardState.noteOff (1, noteNumber, 1.0f);
}

} // namespace foleys
