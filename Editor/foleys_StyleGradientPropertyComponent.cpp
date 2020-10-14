/*
 ==============================================================================
    Copyright (c) 2019-2020 Foleys Finest Audio Ltd. - Daniel Walz
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


StyleGradientPropertyComponent::StyleGradientPropertyComponent (MagicGUIBuilder& builderToUse,
                                                                juce::Identifier propertyToUse,
                                                                juce::ValueTree& nodeToUse)
  : StylePropertyComponent (builderToUse, propertyToUse, nodeToUse)
{
    auto label = std::make_unique<juce::Label>();
    label->setEditable (true);

    addAndMakeVisible (label.get());

    variables.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);
    addAndMakeVisible (variables);

    label->getTextValue().addListener (this);
    label->onTextChange = [&]
    {
        if (auto* l = dynamic_cast<juce::Label*>(editor.get()))
            node.setProperty (property, l->getText(), &builder.getUndoManager());

        refresh();
    };

    variables.onClick = [&]
    {
        juce::PopupMenu menu;
        juce::Component::SafePointer<juce::Label> l = dynamic_cast<juce::Label*>(editor.get());
        for (auto v : builder.getStylesheet().getPaletteEntryNames())
            menu.addItem (v, [l, v]() mutable { if (l) l->setText ("$" + v, juce::sendNotification); });

        menu.showAt (editor.get());
    };

    mouseEvents.onMouseDown = [this](const juce::MouseEvent&)
    {
        auto currentColour = juce::Colours::black;
        if (auto* l = dynamic_cast<juce::Label*>(editor.get()))
        {
            if (l->getText().isNotEmpty())
            {
                currentColour = builder.getStylesheet().getColour (l->getText());
            }
            else
            {
                if (auto* lookandfeel = builder.getStylesheet().getLookAndFeel (node))
                {
//                    auto id = builder.findColourId (node.getType(), property);
//                    if (id >= 0)
//                        currentColour = lookandfeel->findColour (id);
                }
            }
        }

        auto newColourPanel = std::make_unique<GradientPanel>(gradient);
        newColourPanel->addChangeListener (this);
        newColourPanel->setSize (300, 500);
        colourPanel = newColourPanel.get();
#if JUCE_VERSION > 0x60001
        juce::CallOutBox::launchAsynchronously (std::move (newColourPanel), getScreenBounds(), nullptr);
#else
        juce::CallOutBox::launchAsynchronously (newColourPanel.release(), getScreenBounds(), nullptr);
#endif
    };

    label->getTextValue().addListener (this);

    editor = std::move (label);

    mouseEvents.attachTo (editor.get());
}

StyleGradientPropertyComponent::~StyleGradientPropertyComponent()
{
    if (colourPanel)
        colourPanel->removeChangeListener (this);
}

void StyleGradientPropertyComponent::refresh()
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
            label->setText (value.toString(), juce::dontSendNotification);
        }
    }

    repaint();
}

void StyleGradientPropertyComponent::valueChanged (juce::Value& value)
{
    gradient.setup (value.getValue().toString(), builder.getStylesheet());
}

void StyleGradientPropertyComponent::changeListenerCallback (juce::ChangeBroadcaster* sender)
{
    if (auto* selector = dynamic_cast<juce::ColourSelector*>(sender))
    {
        node.setProperty (property, gradient.toString(), &builder.getUndoManager());
        refresh();
    }
}

void StyleGradientPropertyComponent::resized()
{
    auto b = getLocalBounds().reduced (1).withLeft (getWidth() / 2);
    remove.setBounds (b.removeFromRight (getHeight()));
    variables.setBounds (b.removeFromRight (getHeight()));

    if (editor)
        editor->setBounds (b);
}

//==============================================================================

StyleGradientPropertyComponent::GradientPanel::GradientPanel (GradientBackground& gradientToUse)
  : gradient (gradientToUse)
{
    typeSelect.addItemList ({"None", "linear", "radial"}, 1);
    addAndMakeVisible (typeSelect);
    typeSelect.setSelectedId (gradient.type + 1, juce::dontSendNotification);
    typeSelect.onChange = [&]
    {
        switch (typeSelect.getSelectedId())
        {
            case 1:
                gradient.type = GradientBackground::none;
                break;
            case 2:
                gradient.type = GradientBackground::linear;
                break;
            case 3:
                gradient.type = GradientBackground::radial;
                break;
            default:
                break;
        }
        selector.sendChangeMessage();
    };

    angle.setRange (0, 360, 1);
    addAndMakeVisible (angle);
    angle.setValue (juce::radiansToDegrees (gradient.angle));
    angle.onValueChange = [&]
    {
        gradient.angle = juce::degreesToRadians (angle.getValue());
        selector.sendChangeMessage();
    };

    addAndMakeVisible (close);
    addAndMakeVisible (selector);

    close.onClick = [this]
    {
        if (auto* box = findParentComponentOfClass<juce::CallOutBox>())
            box->dismiss();
    };
}

void StyleGradientPropertyComponent::GradientPanel::resized()
{
    auto area = getLocalBounds();
    auto header = area.removeFromTop (24);
    close.setBounds (header.removeFromRight (24));
    typeSelect.setBounds (header);
    angle.setBounds (area.removeFromTop (24));

    selector.setBounds (area);
}

void StyleGradientPropertyComponent::GradientPanel::addChangeListener (juce::ChangeListener* listener)
{
    selector.addChangeListener (listener);
}

void StyleGradientPropertyComponent::GradientPanel::removeChangeListener (juce::ChangeListener* listener)
{
    selector.removeChangeListener (listener);
}

//==============================================================================

//namespace IDs
//{
//    static juce::String swatches { "swatches" };
//    static juce::String colour   { "colour" };
//}

StyleGradientPropertyComponent::GradientPanel::ColourSelectorWithSwatches::ColourSelectorWithSwatches()
{
    properties.setStorageParameters (ToolBox::getApplicationPropertyStorage());
    swatchColours.resize (32, juce::Colour());

    loadSwatches();
}

StyleGradientPropertyComponent::GradientPanel::ColourSelectorWithSwatches::~ColourSelectorWithSwatches()
{
    saveSwatches();
}

int StyleGradientPropertyComponent::GradientPanel::ColourSelectorWithSwatches::getNumSwatches() const
{
    return static_cast<int>(swatchColours.size());
}

juce::Colour StyleGradientPropertyComponent::GradientPanel::ColourSelectorWithSwatches::getSwatchColour (int index) const
{
    return swatchColours [size_t (index)];
}

void StyleGradientPropertyComponent::GradientPanel::ColourSelectorWithSwatches::setSwatchColour (int index, const juce::Colour& colour)
{
    swatchColours [size_t (index)] = colour;
}

void StyleGradientPropertyComponent::GradientPanel::ColourSelectorWithSwatches::loadSwatches()
{
    if (auto* p = properties.getUserSettings())
    {
        auto coloursNode = (p->containsKey (IDs::swatches)) ? p->getXmlValue (IDs::swatches)
                                                            : p->createXml (IDs::swatches);

        for (int i = 0; i < std::min (coloursNode->getNumChildElements(), int (swatchColours.size())); ++i)
            swatchColours [size_t (i)] = juce::Colour::fromString (coloursNode->getChildElement (i)->getAllSubText());
    }
}

void StyleGradientPropertyComponent::GradientPanel::ColourSelectorWithSwatches::saveSwatches()
{
    if (auto* p = properties.getUserSettings())
    {
        auto coloursNode = (p->containsKey (IDs::swatches)) ? p->getXmlValue (IDs::swatches)
                                                            : p->createXml (IDs::swatches);

        coloursNode->deleteAllChildElements();
        for (int i = 0; i < int (swatchColours.size()); ++i)
        {
            auto* node = coloursNode->createNewChildElement (IDs::colour);
            node->addTextElement (swatchColours [size_t (i)].toDisplayString (true));
        }

        p->setValue (IDs::swatches, coloursNode.get());
        p->setNeedsToBeSaved (true);
    }
}


} // namespace foleys
