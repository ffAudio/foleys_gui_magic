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

Decorator::Decorator (MagicGUIBuilder& builder, juce::ValueTree node, std::unique_ptr<juce::Component> wrapped)
  : magicBuilder (builder),
    configNode (node),
    component (std::move (wrapped))
{
    setOpaque (false);

    visibility.addListener (this);

    setInterceptsMouseClicks (false, true);

    if (component.get() != nullptr)
        addAndMakeVisible (component.get());
}

void Decorator::setEditMode (bool shouldEdit)
{
    setInterceptsMouseClicks (shouldEdit, true);

    if (component.get() != nullptr)
        component->setInterceptsMouseClicks (!shouldEdit, !shouldEdit);
}

void Decorator::configureDecorator (Stylesheet& stylesheet, const juce::ValueTree& node)
{
    if (auto* lookAndFeel = stylesheet.getLookAndFeel (node))
        setLookAndFeel (lookAndFeel);

    auto* processorState = magicBuilder.getProcessorState();

    auto visibilityNode = magicBuilder.getStyleProperty (IDs::visibility, node);
    if (! visibilityNode.isVoid() && processorState)
        visibility.referTo (processorState->getPropertyAsValue (visibilityNode.toString()));

    auto bg = magicBuilder.getStyleProperty (IDs::backgroundColour, node);
    if (! bg.isVoid())
        backgroundColour = Stylesheet::parseColour (bg.toString());

    auto bcVar = magicBuilder.getStyleProperty (IDs::borderColour, node);
    if (! bcVar.isVoid())
        borderColour = Stylesheet::parseColour (bcVar.toString());

    auto borderVar = magicBuilder.getStyleProperty (IDs::border, node);
    if (! borderVar.isVoid())
        border = static_cast<float> (borderVar);

    auto marginVar = magicBuilder.getStyleProperty (IDs::margin, node);
    if (! marginVar.isVoid())
        margin = static_cast<float> (marginVar);

    auto paddingVar = magicBuilder.getStyleProperty (IDs::padding, node);
    if (! paddingVar.isVoid())
        padding = static_cast<float> (paddingVar);

    auto radiusVar = magicBuilder.getStyleProperty (IDs::radius, node);
    if (! radiusVar.isVoid())
        radius = static_cast<float> (radiusVar);

    caption    = node.getProperty (IDs::caption, juce::String());
    tabCaption = node.getProperty (IDs::tabCaption, juce::String());
    auto tc    = magicBuilder.getStyleProperty (IDs::tabColour, node);
    if (! tc.isVoid())
        tabColour = Stylesheet::parseColour (tc.toString());

    auto sizeVar = magicBuilder.getStyleProperty (IDs::captionSize, node);
    if (! sizeVar.isVoid())
        captionSize = static_cast<float> (sizeVar);

    auto ccVar = magicBuilder.getStyleProperty (IDs::captionColour, node);
    if (! ccVar.isVoid())
        captionColour = Stylesheet::parseColour (ccVar.toString());

    auto placementVar = magicBuilder.getStyleProperty (IDs::captionPlacement, node);
    if (! placementVar.isVoid())
        justification = juce::Justification (MagicGUIBuilder::makeJustificationsChoices()[placementVar.toString()]);

    backgroundImage = stylesheet.getBackgroundImage (node);
    backgroundFill  = stylesheet.getBackgroundGradient (node);

    auto alphaVar = magicBuilder.getStyleProperty (IDs::backgroundAlpha, node);
    if (! alphaVar.isVoid())
        backgroundAlpha = static_cast<float> (alphaVar);

    auto backPlacement = magicBuilder.getStyleProperty (IDs::imagePlacement, node);
    if (! backPlacement.isVoid())
    {
        if (backPlacement.toString() == IDs::imageStretch)
            backgroundPlacement = juce::RectanglePlacement::stretchToFit;
        else if (backPlacement.toString() == IDs::imageFill)
            backgroundPlacement = juce::RectanglePlacement::fillDestination;
        else if (backPlacement.toString() == IDs::imageCentred)
            backgroundPlacement = juce::RectanglePlacement::centred;
    }

    if (component)
    {
        if (auto* tooltipClient = dynamic_cast<juce::SettableTooltipClient*>(component.get()))
        {
            auto tooltip = magicBuilder.getStyleProperty (IDs::tooltip, node).toString();
            if (tooltip.isNotEmpty())
                tooltipClient->setTooltip (tooltip);
        }
    }
}

void Decorator::configureComponent (Stylesheet& stylesheet, const juce::ValueTree& node)
{
    if (component.get() == nullptr)
        return;

    for (const auto& p : magicBuilder.getSettableProperties (node.getType()))
    {
        auto value = stylesheet.getStyleProperty (p->name, node);
        if (value.isVoid() == false)
            p->set (component.get(), value);
    }
}

void Decorator::configureFlexBoxItem (const juce::ValueTree& node)
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

void Decorator::paint (juce::Graphics& g)
{
    juce::Graphics::ScopedSaveState stateSave (g);

    const auto bounds = getLocalBounds().toFloat().reduced (margin);

    {
        juce::Graphics::ScopedSaveState save (g);
        g.setColour (backgroundColour);

        if (backgroundFill.size() > 1)
            g.setGradientFill (juce::ColourGradient::vertical (backgroundFill.getFirst(), backgroundFill.getLast(), bounds));

        if (radius > 0.0f)
            g.fillRoundedRectangle (bounds, radius);
        else
            g.fillRect (bounds);
    }

    if (! backgroundImage.isNull())
    {
        juce::Graphics::ScopedSaveState save (g);
        g.setOpacity (backgroundAlpha);
        g.drawImage (backgroundImage, bounds, backgroundPlacement);
    }

    if (border > 0.0f)
    {
        g.setColour (borderColour);

        if (radius > 0.0f)
            g.drawRoundedRectangle (bounds, radius, border);
        else
            g.drawRect (bounds, border);
    }

    if (caption.isNotEmpty())
    {
        g.setColour (captionColour);
        auto box = getLocalBounds().reduced (int (margin + padding));
        if (justification.getOnlyVerticalFlags() & juce::Justification::top)
            box.setHeight (int (captionSize));
        else
            box.setTop (int (box.getBottom() - captionSize));

        g.setFont (box.getHeight() * 0.8f);
        g.drawFittedText (caption, box, justification.getOnlyHorizontalFlags(), 1);
    }
}

juce::Rectangle<int> Decorator::getClientBounds() const
{
    auto box = getLocalBounds().reduced (juce::roundToInt (margin + padding));
    if (caption.isNotEmpty())
    {
        if (justification.getOnlyVerticalFlags() & juce::Justification::top)
            box.removeFromTop (int (captionSize));
        else
            box.removeFromBottom (int (captionSize));
    }

    return box;
}

void Decorator::resized()
{
    if (component.get() == nullptr)
        return;

    auto box = getLocalBounds().reduced (int (margin + padding));
    if (caption.isNotEmpty())
    {
        if (justification.getOnlyVerticalFlags() & juce::Justification::top)
            box.removeFromTop (int (captionSize));
        else
            box.removeFromBottom (int (captionSize));
    }

    component->setBounds (box);
}

void Decorator::updateLayout()
{
    resized();
}

juce::Component* Decorator::getWrappedComponent()
{
    return component.get();
}

const juce::ValueTree& Decorator::getConfigNode() const
{
    return configNode;
}

void Decorator::valueChanged (juce::Value& source)
{
    if (source == visibility)
        setVisible (visibility.getValue());
}

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
void Decorator::paintOverChildren (juce::Graphics& g)
{
    if (magicBuilder.isEditModeOn() && magicBuilder.getSelectedNode() == configNode)
    {
        g.setColour (juce::Colours::orange.withAlpha (0.5f));
        g.fillRoundedRectangle (getLocalBounds().toFloat(), 5.0f);
    }
}

void Decorator::mouseDown (const juce::MouseEvent&)
{
    magicBuilder.setSelectedNode (configNode);
}

void Decorator::mouseDrag (const juce::MouseEvent& event)
{
    if (event.mouseWasDraggedSinceMouseDown())
    {
        auto* container = juce::DragAndDropContainer::findParentDragContainerFor (this);
        container->startDragging (IDs::dragSelected, this);
    }
}

bool Decorator::isInterestedInDragSource (const juce::DragAndDropTarget::SourceDetails &)
{
    return true;
}

void Decorator::itemDropped (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    if (dragSourceDetails.description == IDs::dragSelected)
    {
        auto dragged = magicBuilder.getSelectedNode();
        if (dragged.isValid() == false)
            return;

        magicBuilder.draggedItemOnto (dragged, configNode);
    }

    auto node = juce::ValueTree::fromXml (dragSourceDetails.description.toString());
    if (node.isValid())
        magicBuilder.draggedItemOnto (node, configNode);
}
#endif


}
