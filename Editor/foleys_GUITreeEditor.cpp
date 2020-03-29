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

GUITreeEditor::GUITreeEditor (MagicGUIBuilder& builderToEdit)
  : builder (builderToEdit),
    undo (builder.getUndoManager())
{
    treeView.setRootItemVisible (true);
    treeView.setMultiSelectEnabled (false);

    setValueTree (tree);

    addAndMakeVisible (treeView);
}

void GUITreeEditor::paint (juce::Graphics& g)
{
    g.setColour (EditorColours::outline);
    g.drawRect (getLocalBounds(), 1);
    g.setColour (EditorColours::text);
    g.drawFittedText (TRANS ("GUI tree"), 2, 2, getWidth() - 4, 20, juce::Justification::centred, 1);
}

void GUITreeEditor::resized()
{
    auto bounds = getLocalBounds().reduced (1);
    bounds.removeFromTop (24);

    treeView.setBounds (bounds);
}

void GUITreeEditor::setValueTree (juce::ValueTree& refTree)
{
    auto restorer = treeView.getRootItem() != nullptr ? treeView.getOpennessState (true)
                                                      : std::unique_ptr<juce::XmlElement>();

    tree = refTree;
    tree.addListener (this);

    treeView.setRootItem (nullptr);
    if (tree.isValid())
    {
        rootItem = std::make_unique<GUITreeEditor::GuiTreeItem> (builder, tree);
        treeView.setRootItem (rootItem.get());
    }

    if (restorer.get() != nullptr)
        treeView.restoreOpennessState (*restorer, true);
}

void GUITreeEditor::updateTree()
{
    auto guiNode = builder.getConfigTree().getChildWithName (IDs::view);
    setValueTree (guiNode);
}

void GUITreeEditor::setSelectedNode (const juce::ValueTree& node)
{
    if (rootItem.get() == nullptr || node.isAChildOf (tree) == false)
        return;

    std::stack<int> path;
    auto probe = node;
    while (probe != tree)
    {
        auto parent = probe.getParent();
        path.push (parent.indexOf (probe));
        probe = parent;
    }

    juce::TreeViewItem* itemToSelect = rootItem.get();
    while (path.empty() ==  false)
    {
        itemToSelect->setOpen (true);
        auto* childItem = itemToSelect->getSubItem (path.top());
        path.pop();
        itemToSelect = childItem;
    }

    itemToSelect->setSelected (true, true, juce::dontSendNotification);
    treeView.scrollToKeepItemVisible (itemToSelect);
}

void GUITreeEditor::valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&)
{
    updateTree();
}

void GUITreeEditor::valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&)
{
    updateTree();
}

void GUITreeEditor::valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int)
{
    updateTree();
}

void GUITreeEditor::valueTreeChildOrderChanged (juce::ValueTree&, int, int)
{
    updateTree();
}

void GUITreeEditor::valueTreeParentChanged (juce::ValueTree&)
{
    updateTree();
}

//==============================================================================

GUITreeEditor::GuiTreeItem::GuiTreeItem (MagicGUIBuilder& builderToUse, juce::ValueTree& refValueTree)
  : builder (builderToUse),
    itemNode (refValueTree)
{
}

juce::String GUITreeEditor::GuiTreeItem::getUniqueName() const
{
    const auto* parent = getParentItem();
    auto name = (parent != nullptr) ? parent->getUniqueName() + "|" + itemNode.getType().toString()
                                    : itemNode.getType().toString();
    auto parentNode = itemNode.getParent();
    if (parentNode.isValid())
        name += "(" + juce::String (parentNode.indexOf (itemNode)) + ")";

    return name;
}

bool GUITreeEditor::GuiTreeItem::mightContainSubItems()
{
    return itemNode.getNumChildren() > 0;
}

void GUITreeEditor::GuiTreeItem::paintItem (juce::Graphics& g, int width, int height)
{
    if (isSelected())
        g.fillAll (EditorColours::selectedBackground.withAlpha (0.5f));

    g.setColour (EditorColours::text);
    g.setFont (height * 0.7f);

    juce::String name = itemNode.getType().toString();

    if (itemNode.hasProperty (IDs::id))
        name += " (" + itemNode.getProperty (IDs::id).toString() + ")";

    if (itemNode.hasProperty (IDs::caption))
        name += ": " + itemNode.getProperty (IDs::caption).toString();

    g.drawText (name, 4, 0, width - 4, height, juce::Justification::centredLeft, true);
}

void GUITreeEditor::GuiTreeItem::itemOpennessChanged (bool isNowOpen)
{
    if (isNowOpen && getNumSubItems() == 0)
    {
        for (auto child : itemNode)
            addSubItem (new GuiTreeItem (builder, child));
    }
}

void GUITreeEditor::GuiTreeItem::itemSelectionChanged (bool isNowSelected)
{
    if (isNowSelected)
        builder.setSelectedNode (itemNode);
}

juce::var GUITreeEditor::GuiTreeItem::getDragSourceDescription()
{
    return IDs::dragSelected;
}

bool GUITreeEditor::GuiTreeItem::isInterestedInDragSource (const juce::DragAndDropTarget::SourceDetails&)
{
    return itemNode.getType() == IDs::view;
}

void GUITreeEditor::GuiTreeItem::itemDropped (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails, int index)
{
    const auto text = dragSourceDetails.description.toString();
    if (text == IDs::dragSelected)
    {
        auto selectedNode = builder.getSelectedNode();
        builder.draggedItemOnto (selectedNode, itemNode, index);
        return;
    }

    const auto node = juce::ValueTree::fromXml (text);
    if (node.isValid())
    {
        builder.draggedItemOnto (node, itemNode, index);
        return;
    }
}


} // namespace foleys
