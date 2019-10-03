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
    treeView.setRootItemVisible (true);
    treeView.setMultiSelectEnabled (false);

    addNode.setEnabled (false);
    addNode.onClick = [&]
    {
        // Show popup with types of nodes to add to the ValueTree: div, slider, button, etc
        // add new node as a child of the currently selected node
    };

    removeNode.onClick = [&]
    {
        if (GUITreeEditor::GuiTreeItem* selectedItem = static_cast<GUITreeEditor::GuiTreeItem*> (treeView.getSelectedItem (0)))
        {
            juce::ValueTree& tree = selectedItem->getTree();
            tree.getParent().removeChild (tree, nullptr);
        }
    };

    setValueTree (tree);

    addAndMakeVisible (treeView);
    addAndMakeVisible (addNode);
    addAndMakeVisible (removeNode);
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

    auto bottomBarBounds = bounds.removeFromBottom (24).reduced (10, 0);
    removeNode.setBounds (bottomBarBounds.removeFromRight (24));
    addNode   .setBounds (bottomBarBounds.removeFromRight (24));

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
        rootItem = std::make_unique<GUITreeEditor::GuiTreeItem> (*this, tree);
        treeView.setRootItem (rootItem.get());
    }

    if (restorer.get() != nullptr)
        treeView.restoreOpennessState (*restorer, true);
}

void GUITreeEditor::updateTree()
{
    setValueTree (tree);
}

void GUITreeEditor::valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                              const juce::Identifier& property)
{
    updateTree();
}

void GUITreeEditor::valueTreeChildAdded (juce::ValueTree& parentTree,
                                         juce::ValueTree& childWhichHasBeenAdded)
{
    updateTree();
}

void GUITreeEditor::valueTreeChildRemoved (juce::ValueTree& parentTree,
                                           juce::ValueTree& childWhichHasBeenRemoved,
                                           int indexFromWhichChildWasRemoved)
{
    updateTree();
}

void GUITreeEditor::valueTreeChildOrderChanged (juce::ValueTree& parentTreeWhoseChildrenHaveMoved,
                                                int oldIndex, int newIndex)
{
}

void GUITreeEditor::valueTreeParentChanged (juce::ValueTree& treeWhoseParentHasChanged)
{
    updateTree();
}

//==============================================================================

GUITreeEditor::GuiTreeItem::GuiTreeItem (GUITreeEditor& refGuiTreeEditor, juce::ValueTree& refValueTree)
  : guiTreeEditor (refGuiTreeEditor),
    tree (refValueTree)
{
}

juce::String GUITreeEditor::GuiTreeItem::getUniqueName() const
{
    const auto* parent = getParentItem();
    auto name = (parent != nullptr) ? parent->getUniqueName() + "|" + tree.getType().toString()
                                    : tree.getType().toString();
    auto parentNode = tree.getParent();
    if (parentNode.isValid())
        name += "(" + juce::String (parentNode.indexOf (tree)) + ")";

    return name;
}

bool GUITreeEditor::GuiTreeItem::mightContainSubItems()
{
    return tree.getNumChildren() > 0;
}

void GUITreeEditor::GuiTreeItem::paintItem (juce::Graphics& g, int width, int height)
{
    if (isSelected())
        g.fillAll ({91, 103, 109});

    g.setColour (juce::Colours::white);
    g.setFont (height * 0.7f);

    juce::String name = tree.getType().toString();

    if (tree.hasProperty (IDs::id))
        name += " (" + tree.getProperty (IDs::id).toString() + ")";

    if (tree.hasProperty (IDs::caption))
        name += ": " + tree.getProperty (IDs::caption).toString();

    g.drawText (name, 4, 0, width - 4, height, juce::Justification::centredLeft, true);
}

void GUITreeEditor::GuiTreeItem::itemOpennessChanged (bool isNowOpen)
{
    if (isNowOpen && getNumSubItems() == 0)
    {
        for (auto child : tree)
            addSubItem (new GuiTreeItem (guiTreeEditor, child));
    }
}

void GUITreeEditor::GuiTreeItem::itemSelectionChanged (bool isNowSelected)
{
    if (isNowSelected && guiTreeEditor.onSelectionChanged != nullptr)
        guiTreeEditor.onSelectionChanged (tree);
}


} // namespace foleys
