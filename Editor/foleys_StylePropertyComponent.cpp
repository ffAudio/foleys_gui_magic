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


StylePropertyComponent::StylePropertyComponent (MagicBuilder& builderToUse, juce::Identifier propertyToUse, juce::ValueTree& nodeToUse)
  : juce::PropertyComponent (propertyToUse.toString()),
    builder (builderToUse),
    property (propertyToUse),
    node (nodeToUse)
{
    addAndMakeVisible (remove);

    remove.setColour (juce::TextButton::buttonColourId, EditorColours::removeButton);
    remove.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);
    remove.onClick = [&]
    {
        node.removeProperty (property, undo);
        refresh();
    };
}

void StylePropertyComponent::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().reduced (1).withWidth (getWidth() / 2);

    g.fillAll (EditorColours::background);
    g.setColour (EditorColours::outline);
    g.drawHorizontalLine (0, 0, getRight());
    g.drawHorizontalLine (getBottom() - 1, 0, getRight());
    g.setColour (inherited ? EditorColours::disabledText : EditorColours::text);
    g.drawFittedText (property.toString(), b, juce::Justification::left, 1);
}

void StylePropertyComponent::resized()
{
    auto b = getLocalBounds().reduced (1).withLeft (getWidth() / 2);
    remove.setBounds (b.removeFromRight (getHeight()));
    editor->setBounds (b);
}

void StylePropertyComponent::setTooltipForNode (juce::ValueTree& definedHere)
{
    if (definedHere == node)
    {
        setTooltip ({});
        remove.setEnabled (true);
        return;
    }

    const auto& s = builder.getStylesheet();
    if (s.isClassNode (definedHere))
        setTooltip ("Class: " + definedHere.getType().toString());
    else if (s.isTypeNode (definedHere))
        setTooltip ("Type: " + definedHere.getType().toString());
    else if (s.isIdNode (definedHere))
        setTooltip ("Node: " + definedHere.getType().toString());
    else
        setTooltip (definedHere.getType().toString());

    remove.setEnabled (false);
}

} // namespace foleys
