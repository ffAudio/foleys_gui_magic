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

GUITreeEditor::GUITreeEditor (MagicBuilder& builderToEdit)
  : builder (builderToEdit)
{
}

void GUITreeEditor::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::silver);
    g.drawRect (getLocalBounds(), 1);
    g.setColour (juce::Colours::white);
    g.drawFittedText (TRANS ("GUI tree"), 2, 2, getWidth() - 4, 20, juce::Justification::centred, 1);
}

void GUITreeEditor::resized()
{
    auto bounds = getLocalBounds().reduced (1);
    bounds.removeFromTop (24);
    treeView.setBounds (bounds);
}

void GUITreeEditor::valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                              const juce::Identifier& property)
{
}

void GUITreeEditor::valueTreeChildAdded (juce::ValueTree& parentTree,
                                         juce::ValueTree& childWhichHasBeenAdded)
{
}

void GUITreeEditor::valueTreeChildRemoved (juce::ValueTree& parentTree,
                                           juce::ValueTree& childWhichHasBeenRemoved,
                                           int indexFromWhichChildWasRemoved)
{
}

void GUITreeEditor::valueTreeChildOrderChanged (juce::ValueTree& parentTreeWhoseChildrenHaveMoved,
                                                int oldIndex, int newIndex)
{
}

void GUITreeEditor::valueTreeParentChanged (juce::ValueTree& treeWhoseParentHasChanged)
{
}


} // namespace foleys
