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

namespace foleys
{

GuiItem::GuiItem (MagicGUIBuilder& builder, juce::ValueTree node)
  : magicBuilder (builder),
    configNode (node)
{
    setOpaque (false);
    setInterceptsMouseClicks (false, true);

    visibility.addListener (this);
    configNode.addListener (this);
}

void GuiItem::setColourTranslation (std::vector<std::pair<juce::String, int>> mapping)
{
    colourTranslation = mapping;
}

juce::StringArray GuiItem::getColourNames() const
{
    juce::StringArray names;

    for (const auto& pair : colourTranslation)
        names.addIfNotAlreadyThere (pair.first);

    return names;
}

juce::var GuiItem::getProperty (const juce::Identifier& property)
{
    return magicBuilder.getStyleProperty (property, configNode);
}

MagicGUIState& GuiItem::getMagicState()
{
    return magicBuilder.getMagicState();
}

GuiItem* GuiItem::findGuiItemWithId (const juce::String& name)
{
    if (configNode.getProperty (IDs::id, juce::String()).toString() == name)
        return this;

    return nullptr;
}

void GuiItem::updateInternal()
{
    auto& stylesheet = magicBuilder.getStylesheet();

    if (auto* lookAndFeel = stylesheet.getLookAndFeel (configNode))
        setLookAndFeel (lookAndFeel);

    decorator.configure (magicBuilder, configNode);
    configureComponent();
    configureFlexBoxItem (configNode);

    updateColours();

    update();

    repaint();
}

void GuiItem::updateColours()
{
    decorator.updateColours (magicBuilder, configNode);

    auto* component = getWrappedComponent();
    if (component == nullptr)
        return;

    for (auto& pair : colourTranslation)
    {
        auto colour = magicBuilder.getStyleProperty (pair.first, configNode).toString();
        if (colour.isNotEmpty())
            component->setColour (pair.second, magicBuilder.getStylesheet().getColour (colour));
    }
}

void GuiItem::configureComponent()
{
    auto* component = getWrappedComponent();
    if (component == nullptr)
        return;

    component->setComponentID (configNode.getProperty (IDs::id, juce::String()).toString());

    if (auto* tooltipClient = dynamic_cast<juce::SettableTooltipClient*>(component))
    {
        auto tooltip = magicBuilder.getStyleProperty (IDs::tooltip, configNode).toString();
        if (tooltip.isNotEmpty())
            tooltipClient->setTooltip (tooltip);
    }

    auto  visibilityNode = magicBuilder.getStyleProperty (IDs::visibility, configNode);
    if (! visibilityNode.isVoid())
        visibility.referTo (magicBuilder.getMagicState().getPropertyAsValue (visibilityNode.toString()));
}

void GuiItem::configureFlexBoxItem (const juce::ValueTree& node)
{
    flexItem = juce::FlexItem (*this).withFlex (1.0f);

    const auto minWidth = magicBuilder.getStyleProperty (IDs::minWidth, node);
    if (! minWidth.isVoid())
        flexItem.minWidth = minWidth;

    const auto maxWidth = magicBuilder.getStyleProperty (IDs::maxWidth, node);
    if (! maxWidth.isVoid())
        flexItem.maxWidth = maxWidth;

    const auto minHeight = magicBuilder.getStyleProperty (IDs::minHeight, node);
    if (! minHeight.isVoid())
        flexItem.minHeight = minHeight;

    const auto maxHeight = magicBuilder.getStyleProperty (IDs::maxHeight, node);
    if (! maxHeight.isVoid())
        flexItem.maxHeight = maxHeight;

    const auto width = magicBuilder.getStyleProperty (IDs::width, node);
    if (! width.isVoid())
        flexItem.width = width;

    const auto height = magicBuilder.getStyleProperty (IDs::height, node);
    if (! height.isVoid())
        flexItem.height = height;

    auto grow = magicBuilder.getStyleProperty (IDs::flexGrow, node);
    if (! grow.isVoid())
        flexItem.flexGrow = grow;

    const auto flexShrink = magicBuilder.getStyleProperty (IDs::flexShrink, node);
    if (! flexShrink.isVoid())
        flexItem.flexShrink = flexShrink;

    const auto flexOrder = magicBuilder.getStyleProperty (IDs::flexOrder, node);
    if (! flexOrder.isVoid())
        flexItem.order = flexOrder;

    const auto alignSelf = magicBuilder.getStyleProperty (IDs::flexAlignSelf, node).toString();
    if (alignSelf == IDs::flexStart)
        flexItem.alignSelf = juce::FlexItem::AlignSelf::flexStart;
    else if (alignSelf == IDs::flexEnd)
        flexItem.alignSelf = juce::FlexItem::AlignSelf::flexEnd;
    else if (alignSelf == IDs::flexCenter)
        flexItem.alignSelf = juce::FlexItem::AlignSelf::center;
    else if (alignSelf == IDs::flexAuto)
        flexItem.alignSelf = juce::FlexItem::AlignSelf::autoAlign;
    else
        flexItem.alignSelf = juce::FlexItem::AlignSelf::stretch;
}

void GuiItem::paint (juce::Graphics& g)
{
    decorator.drawDecorator (g, getLocalBounds());
}

juce::Rectangle<int> GuiItem::getClientBounds() const
{
    return decorator.getClientBounds (getLocalBounds()).client;
}

void GuiItem::resized()
{
    if (auto* component = getWrappedComponent())
        component->setBounds (getClientBounds());
}

void GuiItem::updateLayout()
{
    resized();
}

juce::String GuiItem::getTabCaption (const juce::String& defaultName) const
{
    return decorator.getTabCaption (defaultName);
}

juce::Colour GuiItem::getTabColour() const
{
    return decorator.getTabColour();
}

void GuiItem::valueChanged (juce::Value& source)
{
    if (source == visibility)
        setVisible (visibility.getValue());
}

void GuiItem::valueTreePropertyChanged (juce::ValueTree& treeThatChanged, const juce::Identifier&)
{
    if (treeThatChanged == configNode)
    {
        if (auto* parent = dynamic_cast<GuiItem*>(getParentComponent()))
            parent->updateInternal();
        else
            updateInternal();
    }
}

void GuiItem::valueTreeChildAdded (juce::ValueTree& treeThatChanged, juce::ValueTree&)
{
    if (treeThatChanged == configNode)
        createSubComponents();
}

void GuiItem::valueTreeChildRemoved (juce::ValueTree& treeThatChanged, juce::ValueTree&, int)
{
    if (treeThatChanged == configNode)
        createSubComponents();
}

void GuiItem::valueTreeChildOrderChanged (juce::ValueTree& treeThatChanged, int, int)
{
    if (treeThatChanged == configNode)
        createSubComponents();
}

void GuiItem::valueTreeParentChanged (juce::ValueTree& treeThatChanged)
{
    if (treeThatChanged == configNode)
    {
        if (auto* parent = dynamic_cast<GuiItem*>(getParentComponent()))
            parent->updateInternal();
        else
            updateInternal();
    }
}

void GuiItem::itemDragEnter (const juce::DragAndDropTarget::SourceDetails& details)
{
    if (details.description.toString().startsWith (IDs::dragCC))
    {
        auto paramID = getControlledParameterID (details.localPosition);
        if (paramID.isNotEmpty())
            if (auto* parameter = magicBuilder.getMagicState().getParameter (paramID))
                highlight = parameter->getName (64);

        repaint();
    }
}

void GuiItem::itemDragExit (const juce::DragAndDropTarget::SourceDetails& details)
{
    juce::ignoreUnused (details);
    highlight.clear();
    repaint();
}

void GuiItem::paintOverChildren (juce::Graphics& g)
{
#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    if (magicBuilder.isEditModeOn() && magicBuilder.getSelectedNode() == configNode)
    {
        g.setColour (juce::Colours::orange.withAlpha (0.5f));
        g.fillRoundedRectangle (getLocalBounds().toFloat(), 5.0f);
    }
#endif

    if (highlight.isNotEmpty())
    {
        g.setColour (juce::Colours::red);
        g.drawFittedText (highlight, getLocalBounds(), juce::Justification::centred, 3);
    }
}

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE

void GuiItem::setEditMode (bool shouldEdit)
{
    setInterceptsMouseClicks (shouldEdit, true);

    if (auto* component = getWrappedComponent())
        component->setInterceptsMouseClicks (!shouldEdit, !shouldEdit);
}

void GuiItem::mouseDown (const juce::MouseEvent&)
{
    magicBuilder.setSelectedNode (configNode);
}

void GuiItem::mouseDrag (const juce::MouseEvent& event)
{
    if (event.mouseWasDraggedSinceMouseDown())
    {
        auto* container = juce::DragAndDropContainer::findParentDragContainerFor (this);
        container->startDragging (IDs::dragSelected, this);
    }
}
#endif

bool GuiItem::isInterestedInDragSource (const juce::DragAndDropTarget::SourceDetails &)
{
    return true;
}

void GuiItem::itemDropped (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    highlight.clear();

    auto dragString = dragSourceDetails.description.toString();
    if (dragString.startsWith (IDs::dragCC))
    {
        auto number = dragString.substring (IDs::dragCC.length()).getIntValue();
        auto parameterID = getControlledParameterID (dragSourceDetails.localPosition);
        if (number > 0 && parameterID.isNotEmpty())
            if (auto* procState = dynamic_cast<MagicProcessorState*>(&magicBuilder.getMagicState()))
                procState->mapMidiController (number, parameterID);

        repaint();
        return;
    }

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    if (dragSourceDetails.description == IDs::dragSelected)
    {
        auto dragged = magicBuilder.getSelectedNode();
        if (dragged.isValid() == false)
            return;

        magicBuilder.draggedItemOnto (dragged, configNode);
        return;
    }

    auto node = juce::ValueTree::fromXml (dragSourceDetails.description.toString());
    if (node.isValid())
        magicBuilder.draggedItemOnto (node, configNode);
#endif
}


}
