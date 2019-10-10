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

Decorator::Decorator (MagicBuilder& builder, juce::ValueTree node, std::unique_ptr<juce::Component> wrapped)
  : magicBuilder (builder),
    configNode (node),
    component (std::move (wrapped))
{
    setOpaque (margin == 0 && radius == 0);
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
    auto bg = stylesheet.getProperty (IDs::backgroundColour, node);
    if (! bg.isVoid())
        backgroundColour = stylesheet.parseColour (bg.toString());

    auto bcVar = stylesheet.getProperty (IDs::borderColour, node);
    if (! bcVar.isVoid())
        borderColour = stylesheet.parseColour (bcVar.toString());

    auto borderVar = stylesheet.getProperty (IDs::border, node);
    if (! borderVar.isVoid())
        border = static_cast<float> (borderVar);

    auto marginVar = stylesheet.getProperty (IDs::margin, node);
    if (! marginVar.isVoid())
        margin = static_cast<float> (marginVar);

    auto paddingVar = stylesheet.getProperty (IDs::padding, node);
    if (! paddingVar.isVoid())
        padding = static_cast<float> (paddingVar);

    caption = node.getProperty (IDs::caption, juce::String());

    auto sizeVar = stylesheet.getProperty (IDs::captionSize, node);
    if (! sizeVar.isVoid())
        captionSize = static_cast<float> (sizeVar);

    auto ccVar = stylesheet.getProperty (IDs::captionColour, node);
    if (! ccVar.isVoid())
        captionColour = stylesheet.parseColour (ccVar.toString());

    auto placementVar = stylesheet.getProperty (IDs::captionPlacement, node);
    if (! placementVar.isVoid())
        justification = static_cast<float> (placementVar);

    backgroundImage = stylesheet.getBackgroundImage (node);
    backgroundFill  = stylesheet.getBackgroundGradient (node);

    if (component)
        if (auto* lookAndFeel = stylesheet.getLookAndFeel (node))
            component->setLookAndFeel (lookAndFeel);
}

void Decorator::configureComponent (Stylesheet& stylesheet, const juce::ValueTree& node)
{
    if (component.get() == nullptr)
        return;

    const auto properties = magicBuilder.getSettableProperties (node.getType());
    for (const auto& p : properties)
    {
        auto value = stylesheet.getProperty (p.name, node);
        if (value.isVoid())
            value = p.defaultValue;
        p.setter (component.get(), value, p.options);
    }
}

void Decorator::connectToState (const juce::String& paramID, juce::AudioProcessorValueTreeState& state)
{
    if (auto* slider = dynamic_cast<juce::Slider*>(component.get()))
    {
        sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, paramID, *slider);
    }
    else if (auto* combo = dynamic_cast<juce::ComboBox*>(component.get()))
    {
        if (auto* parameter = state.getParameter (paramID))
            combo->addItemList (parameter->getAllValueStrings(), 1);

        comboboxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, paramID, *combo);
    }
    else if (auto* button = dynamic_cast<juce::Button*>(component.get()))
    {
        button->setClickingTogglesState (true);
        buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, paramID, *button);
    }
}

void Decorator::paint (juce::Graphics& g)
{
    juce::Graphics::ScopedSaveState stateSave (g);

    const auto bounds = getLocalBounds().reduced (margin).toFloat();

    if (radius > 0.0f)
    {
        g.setColour (backgroundColour);

        if (backgroundFill.size() > 1)
            g.setGradientFill (juce::ColourGradient::vertical (backgroundFill.getFirst(), backgroundFill.getLast(), bounds));

        g.fillRoundedRectangle (bounds, radius);

        if (! backgroundImage.isNull())
            g.drawImage (backgroundImage, bounds);

        if (border > 0.0f)
        {
            g.setColour (borderColour);
            g.drawRoundedRectangle (bounds, radius, border);
        }
    }
    else
    {
        g.setColour (backgroundColour);

        if (backgroundFill.size() > 1)
            g.setGradientFill (juce::ColourGradient::vertical (backgroundFill.getFirst(), backgroundFill.getLast(), bounds));

        g.fillRect (bounds);

        if (! backgroundImage.isNull())
            g.drawImage (backgroundImage, bounds);

        if (border > 0.0f)
        {
            g.setColour (borderColour);
            g.drawRect (bounds, border);
        }
    }

    if (caption.isNotEmpty())
    {
        g.setColour (captionColour);
        auto box = getLocalBounds().reduced (margin + padding);
        if (justification.getOnlyVerticalFlags() < int (juce::Justification::bottom))
            box.setHeight (captionSize);
        else
            box.setTop (box.getBottom() - captionSize);

        g.drawFittedText (caption, box, justification.getOnlyHorizontalFlags(), 1);
    }
}

juce::Rectangle<int> Decorator::getClientBounds() const
{
    auto box = getLocalBounds().reduced (margin + padding);
    if (caption.isNotEmpty())
    {
        if (justification.getOnlyVerticalFlags() < int (juce::Justification::bottom))
            box.removeFromTop (captionSize);
        else
            box.removeFromBottom (captionSize);
    }

    return box;
}

void Decorator::resized()
{
    if (component.get() == nullptr)
        return;

    auto box = getLocalBounds().reduced (margin + padding);
    if (caption.isNotEmpty())
    {
        if (justification.getOnlyVerticalFlags() < int (juce::Justification::bottom))
            box.removeFromTop (captionSize);
        else
            box.removeFromBottom (captionSize);
    }

    component->setBounds (box);
}

juce::Component* Decorator::getWrappedComponent()
{
    return component.get();
}

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
void Decorator::paintOverChildren (juce::Graphics& g)
{
    if (magicBuilder.isEditModeOn() && magicBuilder.getSelectedNode() == configNode)
        g.fillAll (juce::Colours::orange.withAlpha (0.5f));
}

void Decorator::mouseDown (const juce::MouseEvent& event)
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

bool Decorator::isInterestedInDragSource (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails)
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
