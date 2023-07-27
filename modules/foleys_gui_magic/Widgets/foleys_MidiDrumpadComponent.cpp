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

#include "foleys_MidiDrumpadComponent.h"

namespace foleys
{

MidiDrumpadComponent::MidiDrumpadComponent (juce::MidiKeyboardState& state)
  : keyboardState (state)
{
    setColour (MidiDrumpadComponent::background, juce::Colours::darkgrey);
    setColour (MidiDrumpadComponent::padFill, juce::Colours::green);
    setColour (MidiDrumpadComponent::padDownFill, juce::Colours::lightgreen);
    setColour (MidiDrumpadComponent::padOutline, juce::Colours::darkgreen);
    setColour (MidiDrumpadComponent::padDownOutline, juce::Colours::green);
    setColour (MidiDrumpadComponent::touch, juce::Colours::orange);

    updateButtons();

    startTimerHz (30);
}

MidiDrumpadComponent::~MidiDrumpadComponent()
{
    stopTimer();
}

void MidiDrumpadComponent::setMatrix (int rows, int columns)
{
    numRows = std::max (rows, 1);
    numColumns = std::max (columns, 1);

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
        pads.push_back (std::make_unique<Pad>(*this, rootNote + i));

    for (auto& pad : pads)
        addAndMakeVisible (pad.get());

    resized();
}

void MidiDrumpadComponent::paint (juce::Graphics& g)
{
    g.fillAll (findColour (MidiDrumpadComponent::background));
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

void MidiDrumpadComponent::timerCallback()
{
    if (needsPaint)
    {
        needsPaint = false;
        repaint();
    }
}

//  ==============================================================================

MidiDrumpadComponent::Pad::Pad (MidiDrumpadComponent& ownerToUse, int note)
  : owner (ownerToUse),
    noteNumber (note)
{
    owner.keyboardState.addListener (this);
}

MidiDrumpadComponent::Pad::~Pad()
{
    owner.keyboardState.removeListener (this);
}

void MidiDrumpadComponent::Pad::paint (juce::Graphics& g)
{
    g.setColour (owner.findColour (isDown ? MidiDrumpadComponent::padDownFill : MidiDrumpadComponent::padFill));
    g.fillRoundedRectangle (getLocalBounds().reduced (3).toFloat(), 3.0f);

    g.setColour (owner.findColour (isDown ? MidiDrumpadComponent::padDownOutline : MidiDrumpadComponent::padOutline));
    g.drawRoundedRectangle (getLocalBounds().reduced (3).toFloat(), 3.0f, 1.0f);

    if (isDown)
    {
        auto radius = pressure * 20.0f;
        g.setColour (owner.findColour (MidiDrumpadComponent::touch));
        g.fillEllipse (juce::Rectangle<float>(lastPos.x - 0.5f * radius, lastPos.y - 0.5f * radius, radius, radius));
    }
}

void MidiDrumpadComponent::Pad::mouseDown (const juce::MouseEvent& event)
{
    pressure = event.isPressureValid() ? event.pressure : 1.0f;
    owner.keyboardState.noteOn (1, noteNumber, pressure);
    lastPos = event.getPosition();
    owner.needsPaint = true;
}

void MidiDrumpadComponent::Pad::mouseDrag (const juce::MouseEvent& event)
{
    pressure = event.isPressureValid() ? event.pressure : 1.0f;
    lastPos = event.getPosition();
    owner.needsPaint = true;
}

void MidiDrumpadComponent::Pad::mouseUp (const juce::MouseEvent& event)
{
    pressure = event.isPressureValid() ? event.pressure : 0.0f;
    lastPos = getLocalBounds().getCentre();
    owner.keyboardState.noteOff (1, noteNumber, 1.0f);
    owner.needsPaint = true;
}

void MidiDrumpadComponent::Pad::handleNoteOn (juce::MidiKeyboardState* source,
                                              int midiChannel, int midiNoteNumber, float velocity)
{
    juce::ignoreUnused (source, midiChannel, velocity);

    if (midiNoteNumber != noteNumber)
        return;

    isDown = true;

    owner.needsPaint = true;
}

void MidiDrumpadComponent::Pad::handleNoteOff (juce::MidiKeyboardState* source,
                                               int midiChannel, int midiNoteNumber, float velocity)
{
    juce::ignoreUnused (source, midiChannel, velocity);

    if (midiNoteNumber != noteNumber)
        return;

    isDown = false;

    owner.needsPaint = true;
}

} // namespace foleys
