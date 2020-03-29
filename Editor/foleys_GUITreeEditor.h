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

class MagicGUIBuilder;

/**
 This GUITreeEditor shows the DOM tree for the GUI. The order can be dragged within and nodes be selected
 to edit them in the PropertiesEditor. It also allows copy and paste via CMD+C/V of sub trees.
 */
class GUITreeEditor  : public juce::Component,
                       private juce::ValueTree::Listener
{
public:
    GUITreeEditor (MagicGUIBuilder& builder);

    void paint (juce::Graphics&) override;
    void resized() override;

    void setValueTree (juce::ValueTree& refTree);

    void updateTree();

    void setSelectedNode (const juce::ValueTree& node);

private:
    class GuiTreeItem : public juce::TreeViewItem
    {
    public:
        GuiTreeItem (MagicGUIBuilder& builder, juce::ValueTree& refValueTree);

        juce::String getUniqueName() const override;

        bool mightContainSubItems() override;

        void paintItem (juce::Graphics& g, int width, int height) override;

        void itemOpennessChanged (bool isNowOpen) override;
        void itemSelectionChanged (bool isNowSelected) override;

        juce::var getDragSourceDescription() override;
        bool isInterestedInDragSource (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;
        void itemDropped (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails, int index) override;

        juce::ValueTree& getTree () { return itemNode; }

    private:
        MagicGUIBuilder& builder;
        juce::ValueTree  itemNode;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiTreeItem)
    };


    void valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                   const juce::Identifier& property) override;

    void valueTreeChildAdded (juce::ValueTree& parentTree,
                              juce::ValueTree& childWhichHasBeenAdded) override;

    void valueTreeChildRemoved (juce::ValueTree& parentTree,
                                juce::ValueTree& childWhichHasBeenRemoved,
                                int indexFromWhichChildWasRemoved) override;

    void valueTreeChildOrderChanged (juce::ValueTree& parentTreeWhoseChildrenHaveMoved,
                                     int oldIndex, int newIndex) override;

    void valueTreeParentChanged (juce::ValueTree& treeWhoseParentHasChanged) override;


    MagicGUIBuilder&             builder;
    juce::UndoManager&           undo;

    juce::ValueTree              tree;

    std::unique_ptr<GuiTreeItem> rootItem;
    juce::TreeView               treeView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GUITreeEditor)
};

} // namespace foleys
