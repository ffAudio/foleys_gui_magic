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
  : builder (builderToEdit), tree (builderToEdit.getGuiTree())
{
    tree.addListener (this);
    treeView.setRootItemVisible (false);
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

    treeView.setRootItem (nullptr);
    rootItem = std::make_unique<GUITreeEditor::GuiTreeItem> (*this, tree);
    treeView.setRootItem (rootItem.get());

    addAndMakeVisible (treeView);
    addAndMakeVisible (addNode);
    addAndMakeVisible (removeNode);
}

//#include "../JuceLibraryCode/JuceHeader.h"

void GUITreeEditor::resized()
{
    auto bounds = getLocalBounds().reduced (1);
    treeView.setBounds (bounds);

    auto bottomBarBounds = bounds.removeFromBottom (24).reduced (10, 0);
    removeNode.setBounds (bottomBarBounds.removeFromRight (24));
    addNode   .setBounds (bottomBarBounds.removeFromRight (24));
}

void GUITreeEditor::setValueTree (juce::ValueTree& refTree)
{
    tree = refTree;
    tree.addListener (this);

    treeView.setRootItem (nullptr);
    rootItem = std::make_unique<GUITreeEditor::GuiTreeItem> (*this, tree);
    treeView.setRootItem (rootItem.get());
}

void GUITreeEditor::valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                              const juce::Identifier& property)
{
}

void GUITreeEditor::valueTreeChildAdded (juce::ValueTree& parentTree,
                                         juce::ValueTree& childWhichHasBeenAdded)
{
}

void GUITreeEditor::valueTreeChildRemoved (juce::ValueTree& parentTree,
                                           juce::ValueTree& childWhichHasBeenRemoved,
                                           int indexFromWhichChildWasRemoved)
{
}

void GUITreeEditor::valueTreeChildOrderChanged (juce::ValueTree& parentTreeWhoseChildrenHaveMoved,
                                                int oldIndex, int newIndex)
{
}

void GUITreeEditor::valueTreeParentChanged (juce::ValueTree& treeWhoseParentHasChanged)
{
}

GUITreeEditor::GuiTreeItem::GuiTreeItem (GUITreeEditor& refGuiTreeEditor, juce::ValueTree& refValueTree)
    : guiTreeEditor {refGuiTreeEditor}, tree {refValueTree}
{
}

juce::String GUITreeEditor::GuiTreeItem::getUniqueName() const
{
    return tree.getType().toString();
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

    juce::String s = tree.getType().toString();

    juce::String ss = tree["caption"].toString();
    if (!ss.isEmpty())
        s << " - " << ss;

    g.drawText (s, 4, 0, width - 4, height, juce::Justification::centredLeft, true);
}

void GUITreeEditor::GuiTreeItem::itemOpennessChanged (bool isNowOpen)
{
    if (isNowOpen)
    {
        if (getNumSubItems() == 0)
        {
            for (auto child : tree)
                addSubItem (new GuiTreeItem (guiTreeEditor, child));
        }
    }
}

void GUITreeEditor::GuiTreeItem::itemSelectionChanged (bool isNowSelected)
{
    if (isNowSelected)
    {
        if (guiTreeEditor.onSelectionChanged != nullptr)
            guiTreeEditor.onSelectionChanged (tree);
    }
}


} // namespace foleys
