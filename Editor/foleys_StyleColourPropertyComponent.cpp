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


StyleColourPropertyComponent::StyleColourPropertyComponent (MagicGUIBuilder& builderToUse,
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

    mouseEvents.onMouseDown = [this](const juce::MouseEvent&)
    {
        auto currentColour = juce::Colours::black;
        if (auto* label = dynamic_cast<juce::Label*>(editor.get()))
        {
            if (label->getText().isNotEmpty())
            {
                currentColour = Stylesheet::parseColour (label->getText());
            }
            else
            {
                if (auto* lookandfeel = builder.getStylesheet().getLookAndFeel (node))
                {
                    auto id = builder.findColourId (node.getType(), property);
                    if (id >= 0)
                        currentColour = lookandfeel->findColour (id);
                }
            }
        }

        auto newColourPanel = std::make_unique<ColourPanel>(currentColour);
        newColourPanel->addChangeListener (this);
        newColourPanel->setSize (300, 500);
        colourPanel = newColourPanel.get();
        juce::CallOutBox::launchAsynchronously (newColourPanel.release(), getScreenBounds(), nullptr);
    };

    label->getTextValue().addListener (this);

    editor = std::move (label);

    mouseEvents.attachTo (editor.get());
}

StyleColourPropertyComponent::~StyleColourPropertyComponent()
{
    if (colourPanel)
        colourPanel->removeChangeListener (this);
}

void StyleColourPropertyComponent::refresh()
{
    const auto value = lookupValue();

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
    editor->repaint();
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
    auto colour = Stylesheet::parseColour (value.getValue().toString());
    setColourDisplay (colour);
}

void StyleColourPropertyComponent::changeListenerCallback (juce::ChangeBroadcaster* sender)
{
    if (auto* selector = dynamic_cast<juce::ColourSelector*>(sender))
    {
        const auto newColour = selector->getCurrentColour().toDisplayString (true);
        node.setProperty (property, newColour, &builder.getUndoManager());
        refresh();
    }
}

//==============================================================================

StyleColourPropertyComponent::ColourPanel::ColourPanel (juce::Colour colour)
{
    addAndMakeVisible (close);
    addAndMakeVisible (selector);
    selector.setCurrentColour (colour);

    close.onClick = [this]
    {
        if (auto* box = findParentComponentOfClass<juce::CallOutBox>())
            box->dismiss();
    };
}

void StyleColourPropertyComponent::ColourPanel::resized()
{
    auto area = getLocalBounds();
    close.setBounds (area.removeFromTop (20).withLeft (area.getRight() - 20));
    selector.setBounds (area);
}

void StyleColourPropertyComponent::ColourPanel::addChangeListener (juce::ChangeListener* listener)
{
    selector.addChangeListener (listener);
}

void StyleColourPropertyComponent::ColourPanel::removeChangeListener (juce::ChangeListener* listener)
{
    selector.removeChangeListener (listener);
}

//==============================================================================

namespace IDs
{
    static juce::String swatches { "swatches" };
    static juce::String colour   { "colour" };
}

StyleColourPropertyComponent::ColourPanel::ColourSelectorWithSwatches::ColourSelectorWithSwatches()
{
    properties.setStorageParameters (ToolBox::getApplicationPropertyStorage());
    swatchColours.resize (32, juce::Colour());

    loadSwatches();
}

StyleColourPropertyComponent::ColourPanel::ColourSelectorWithSwatches::~ColourSelectorWithSwatches()
{
    saveSwatches();
}

int StyleColourPropertyComponent::ColourPanel::ColourSelectorWithSwatches::getNumSwatches() const
{
    return static_cast<int>(swatchColours.size());
}

juce::Colour StyleColourPropertyComponent::ColourPanel::ColourSelectorWithSwatches::getSwatchColour (int index) const
{
    return swatchColours [index];
}

void StyleColourPropertyComponent::ColourPanel::ColourSelectorWithSwatches::setSwatchColour (int index, const juce::Colour& colour)
{
    swatchColours [index] = colour;
}

void StyleColourPropertyComponent::ColourPanel::ColourSelectorWithSwatches::loadSwatches()
{
    if (auto* p = properties.getUserSettings())
    {
        auto coloursNode = (p->containsKey (IDs::swatches)) ? p->getXmlValue (IDs::swatches)
                                                            : p->createXml (IDs::swatches);

        for (int i = 0; i < std::min (coloursNode->getNumChildElements(), int (swatchColours.size())); ++i)
            swatchColours [i] = juce::Colour::fromString (coloursNode->getChildElement (i)->getAllSubText());
    }
}

void StyleColourPropertyComponent::ColourPanel::ColourSelectorWithSwatches::saveSwatches()
{
    if (auto* p = properties.getUserSettings())
    {
        auto coloursNode = (p->containsKey (IDs::swatches)) ? p->getXmlValue (IDs::swatches)
                                                            : p->createXml (IDs::swatches);

        coloursNode->deleteAllChildElements();
        for (int i = 0; i < int (swatchColours.size()); ++i)
        {
            auto* node = coloursNode->createNewChildElement (IDs::colour);
            node->addTextElement (swatchColours [i].toDisplayString (true));
        }

        p->setValue (IDs::swatches, coloursNode.get());
        p->setNeedsToBeSaved (true);
    }
}


} // namespace foleys
