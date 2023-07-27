/*
 ==============================================================================
    Copyright (c) 2019-2022 Foleys Finest Audio - Daniel Walz
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

#include "foleys_StyleGradientPropertyComponent.h"

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

    label->getTextValue().addListener (this);
    label->onTextChange = [&]
    {
        if (auto* l = dynamic_cast<juce::Label*>(editor.get()))
            node.setProperty (property, l->getText(), &builder.getUndoManager());

        refresh();
    };

    mouseEvents.onMouseDown = [this](const juce::MouseEvent&)
    {
        auto newColourPanel = std::make_unique<GradientPanel>(gradient);
        newColourPanel->addChangeListener (this);
        newColourPanel->setSize (360, 600);
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
            label->getTextValue().referTo (label->getTextValue());
            label->setText (value.toString(), juce::dontSendNotification);
        }
    }

    repaint();
}

void StyleGradientPropertyComponent::valueChanged (juce::Value& value)
{
    gradient.setup (value.getValue().toString(), builder.getStylesheet());
}

void StyleGradientPropertyComponent::changeListenerCallback (juce::ChangeBroadcaster*)
{
    if (colourPanel)
        colourPanel->colourWasChanged();

    node.setProperty (property, gradient.toString(), &builder.getUndoManager());
    refresh();
}

void StyleGradientPropertyComponent::resized()
{
    auto b = getLocalBounds().reduced (1).withLeft (getWidth() / 2);
    remove.setBounds (b.removeFromRight (getHeight()));

    if (editor)
        editor->setBounds (b);
}

//==============================================================================

StyleGradientPropertyComponent::GradientPanel::GradientPanel (GradientBackground& gradientToUse)
  : gradient (gradientToUse),
    stopSelect (*this)
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
    angle.setTextValueSuffix (juce::String::fromUTF8 ("\u00B0"));
    addAndMakeVisible (angle);
    angle.setValue (juce::radiansToDegrees (gradient.angle));
    angle.onValueChange = [&]
    {
        gradient.angle = juce::degreesToRadians (float (angle.getValue()));
        selector.sendChangeMessage();
    };

    if (!gradient.colours.empty())
        selector.setCurrentColour (gradient.colours.begin()->second);

    addAndMakeVisible (close);
    addAndMakeVisible (selector);
    addAndMakeVisible (stopSelect);

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
    angle.setBounds (area.removeFromTop (24).withLeft (proportionOfWidth (0.2f)).withWidth (proportionOfWidth (0.72f)));
    stopSelect.setBounds (area.removeFromBottom (30).reduced (5, 0));

    selector.setBounds (area);
}

void StyleGradientPropertyComponent::GradientPanel::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::silver);
    g.drawFittedText (TRANS ("angle:"), 0, 24, proportionOfWidth (0.2f), 24, juce::Justification::right, 1);
}

void StyleGradientPropertyComponent::GradientPanel::addChangeListener (juce::ChangeListener* listener)
{
    selector.addChangeListener (listener);
}

void StyleGradientPropertyComponent::GradientPanel::removeChangeListener (juce::ChangeListener* listener)
{
    selector.removeChangeListener (listener);
}

void StyleGradientPropertyComponent::GradientPanel::colourWasChanged()
{
    if (juce::isPositiveAndBelow (stopSelect.selected, int (gradient.colours.size())))
        std::next (gradient.colours.begin(), stopSelect.selected)->second = selector.getCurrentColour();

    stopSelect.repaint();
}

//==============================================================================

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

void StyleGradientPropertyComponent::GradientPanel::ColourSelectorWithSwatches::setSwatchColour (int index, const juce::Colour& newColour)
{
    swatchColours [size_t (index)] = newColour;
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
            auto* colourNode = coloursNode->createNewChildElement (IDs::colour);
            colourNode->addTextElement (swatchColours [size_t (i)].toDisplayString (true));
        }

        p->setValue (IDs::swatches, coloursNode.get());
        p->setNeedsToBeSaved (true);
    }
}

//==============================================================================

StyleGradientPropertyComponent::GradientPanel::GradientStopSelect::GradientStopSelect (GradientPanel& ownerToUse)
  : owner (ownerToUse)
{
}

void StyleGradientPropertyComponent::GradientPanel::GradientStopSelect::paint (juce::Graphics& g)
{
    auto gradientCopy = owner.gradient;
    gradientCopy.type  = GradientBackground::linear;
    gradientCopy.angle = juce::degreesToRadians (270.0f);

    juce::Path p;
    p.addRectangle (getLocalBounds().withTop (5).toFloat());
    gradientCopy.drawGradient (g, getLocalBounds().toFloat(), p);

    int index = 0;
    for (auto& c : gradientCopy.colours)
    {
        auto x      = std::min (juce::roundToInt (c.first * getWidth()), getWidth() - 1);
        auto colour = c.second.withAlpha (1.0f);
        if (index++ == selected)
        {
            g.setColour (colour.darker());
            g.fillRect (x-2, 0, 5, getHeight());
            g.setColour (colour);
            g.fillRect (x, 0, 2, getHeight());
        }
        else
        {
            g.setColour (colour.darker());
            g.fillRect (x-1, 0, 3, getHeight());
            g.setColour (colour);
            g.fillRect (x, 0, 1, getHeight());
        }
    }
}

void StyleGradientPropertyComponent::GradientPanel::GradientStopSelect::mouseDown (const juce::MouseEvent& event)
{
    dragging = getDraggingIndex (event.getPosition().x);
    if (dragging >= 0)
    {
        selected = dragging;
        owner.selector.setCurrentColour (std::next (owner.gradient.colours.begin(), dragging)->second);
    }

    repaint();
}

void StyleGradientPropertyComponent::GradientPanel::GradientStopSelect::mouseMove (const juce::MouseEvent& event)
{
    if (getDraggingIndex (event.getPosition().x) >= 0)
    {
        setMouseCursor (juce::MouseCursor::LeftRightResizeCursor);
        return;
    }

    setMouseCursor (juce::MouseCursor::NormalCursor);
}

void StyleGradientPropertyComponent::GradientPanel::GradientStopSelect::mouseDrag (const juce::MouseEvent& event)
{
    auto& colours = owner.gradient.colours;
    if (dragging > 0 && dragging < int (colours.size()))
    {
        auto it = std::next (colours.begin(), dragging);
        auto newPosition = juce::jlimit (0.0f, 1.0f, event.x / float (getWidth()));
        auto colour = it->second;
        colours.erase (it);
        colours [newPosition] = colour;
        dragging = int (std::distance (colours.begin(), colours.find (newPosition)));
        selected = dragging;
        owner.selector.sendChangeMessage();
        repaint();
    }
}

void StyleGradientPropertyComponent::GradientPanel::GradientStopSelect::mouseUp (const juce::MouseEvent&)
{
    dragging = -1;
    repaint();
}

void StyleGradientPropertyComponent::GradientPanel::GradientStopSelect::mouseDoubleClick (const juce::MouseEvent& event)
{
    auto& colours = owner.gradient.colours;
    auto clicked = getDraggingIndex (event.getPosition().x);
    if (clicked < 0)
    {
        auto newPosition = juce::jlimit (0.0f, 1.0f, event.x / float (getWidth()));
        colours [newPosition] = juce::Colours::red;
        selected = int (std::distance (colours.begin(), colours.find (newPosition)));
        owner.selector.sendChangeMessage();
        repaint();
    }
    else if (clicked > 0 && clicked < int (colours.size()) - 1)
    {
        colours.erase (std::next (colours.begin(), clicked));
        selected = 0;
        owner.selector.sendChangeMessage();
        repaint();
    }
}

int StyleGradientPropertyComponent::GradientPanel::GradientStopSelect::getDraggingIndex (int pos)
{
    int index = 0;
    for (auto& c : owner.gradient.colours)
    {
        if (std::abs (pos - c.first * getWidth()) < 3)
            return index;

        ++index;
    }

    return -1;
}


} // namespace foleys
