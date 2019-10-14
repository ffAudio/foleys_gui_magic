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


StyleColourPropertyComponent::StyleColourPropertyComponent (MagicBuilder& builderToUse,
                                                            juce::Identifier propertyToUse,
                                                            juce::ValueTree& nodeToUse)
  : StylePropertyComponent (builderToUse, propertyToUse, nodeToUse)
{
    auto label = std::make_unique<juce::Label>();
    label->setEditable (true);

    addAndMakeVisible (label.get());

    label->getTextValue().addListener (this);
    label->onTextChange = [&]
    {
        if (auto* label = dynamic_cast<juce::Label*>(editor.get()))
            node.setProperty (property, label->getText(), &builder.getUndoManager());

        refresh();
    };

    editor = std::move (label);
}

void StyleColourPropertyComponent::refresh()
{
    auto value = lookupValue();

    if (auto* label = dynamic_cast<juce::Label*>(editor.get()))
    {
        if (node == inheritedFrom)
        {
            label->getTextValue().referTo (node.getPropertyAsValue (property, &builder.getUndoManager()));
        }
        else
        {
            if (value.isVoid())
                getLookAndFeelColourFallback();

            label->setText (value.toString(), juce::dontSendNotification);
        }
    }

    repaint();
}
    
void StyleColourPropertyComponent::setColourDisplay (juce::Colour colour)
{
    if (colour.isTransparent())
    {
        editor->setColour (juce::Label::backgroundColourId, EditorColours::background);
        editor->setColour (juce::Label::textColourId, EditorColours::text);
    }
    else
    {
        editor->setColour (juce::Label::backgroundColourId, colour);
        editor->setColour (juce::Label::textColourId, colour.contrasting());
    }
}

void StyleColourPropertyComponent::getLookAndFeelColourFallback()
{
    const auto& stylesheet = builder.getStylesheet();
    juce::Colour colour;

    if (auto* lookandfeel = stylesheet.getLookAndFeel (node))
    {
        auto id = builder.findColourId (node.getType(), property);
        if (id >= 0)
            setColourDisplay (lookandfeel->findColour (id));
    }
}

void StyleColourPropertyComponent::valueChanged (juce::Value& value)
{
    auto colour = builder.getStylesheet().parseColour (value.getValue().toString());
    setColourDisplay (colour);
}


} // namespace foleys
