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

ToolBox::ToolBox (juce::Component* parentToUse, MagicBuilder& builderToControl)
  : parent (parentToUse),
    builder (builderToControl)
{
    setOpaque (true);

    saveXml.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);
    loadXml.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);
    saveCSS.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);
    loadCSS.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);

    addAndMakeVisible (saveXml);
    addAndMakeVisible (loadXml);
    addAndMakeVisible (saveCSS);
    addAndMakeVisible (loadCSS);

    addAndMakeVisible (treeView);

    addAndMakeVisible (addNode);
    addAndMakeVisible (removeNode);

    addAndMakeVisible (propertiesPanel);

    saveXml.onClick = [&]
    {
        juce::FileChooser myChooser ("Save XML to file...", getLastLocation(), "*.xml");
        if (myChooser.browseForFileToSave (true))
        {
            juce::File xmlFile (myChooser.getResult());
            juce::FileOutputStream stream (xmlFile);
            stream.setPosition (0);
            stream.truncate();
            stream.writeString (builder.getGuiTree().toXmlString());
            lastLocation = xmlFile;
        }
    };

    loadXml.onClick = [&]
    {
        juce::FileChooser myChooser ("Load from XML file...", getLastLocation(), "*.xml");
        if (myChooser.browseForFileToOpen())
        {
            juce::File xmlFile (myChooser.getResult());
            juce::FileInputStream stream (xmlFile);
            auto loadedTree = juce::ValueTree::fromXml (stream.readEntireStreamAsString());

            if (loadedTree.isValid() && loadedTree.getType() == IDs::magic)
            {
                tree = loadedTree;
                builder.restoreGUI (tree);
                treeView.setRootItem (new GuiTreeItem (tree));
            }

            lastLocation = xmlFile;
        }
    };

    addNode.setEnabled (false);
    addNode.onClick = [&]
    {
        // Show popup with types of nodes to add to the ValueTree: div, slider, button, etc
        // add new node as a child of the currently selected node
    };

    removeNode.onClick = [&]
    {
        if (GuiTreeItem* selectedItem = static_cast<GuiTreeItem*>(treeView.getSelectedItem (0)))
        {
            selectedItem->getParentItem()->removeSubItem (selectedItem->getIndexInParent());
            builder.restoreGUI (tree);
        }
    };

    setBounds (100, 100, 420, 900);
//    addToDesktop (juce::ComponentPeer::StyleFlags::windowIsResizable | juce::ComponentPeer::StyleFlags::windowHasTitleBar);
    addToDesktop (getLookAndFeel().getMenuWindowFlags());

    startTimer (100);

    setVisible (true);

    tree = builderToControl.getGuiTree();
    treeView.setRootItem (new GuiTreeItem (tree));
    treeView.setRootItemVisible (false);

    treeView.setMultiSelectEnabled (false);

}

ToolBox::~ToolBox()
{
    treeView.deleteRootItem();
}

void ToolBox::paint (juce::Graphics& g)
{
    g.fillAll ({170, 169, 173});
}

void ToolBox::resized()
{
    juce::FlexBox topBar;
    topBar.items.add (juce::FlexItem (loadXml).withFlex (1.0f));
    topBar.items.add (juce::FlexItem (saveXml).withFlex (1.0f));
    topBar.items.add (juce::FlexItem (loadCSS).withFlex (1.0f));
    topBar.items.add (juce::FlexItem (saveCSS).withFlex (1.0f));

    juce::FlexBox leftPanel;
    leftPanel.flexDirection = juce::FlexBox::Direction::column;
    leftPanel.items.add (juce::FlexItem (topBar).withFlex (1.0f).withMaxHeight (24.0f));
    leftPanel.items.add (juce::FlexItem (treeView).withFlex (1.0f));

    juce::FlexBox bottomBar;
    bottomBar.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    bottomBar.items.add (juce::FlexItem (addNode).withFlex (1.0f).withMaxHeight (20.0f).withMaxWidth (20.0f));
    bottomBar.items.add (juce::FlexItem (removeNode).withFlex (1.0f).withMaxHeight (20.0f).withMaxWidth (20.0f));

    leftPanel.items.add (juce::FlexItem (bottomBar).withFlex (1.0f).withMaxHeight (20.0f));


    juce::FlexBox rightPanel;
    rightPanel.items.add (juce::FlexItem (propertiesPanel).withFlex (1.0f));


    juce::FlexBox fb;
    fb.items.add (juce::FlexItem (leftPanel).withFlex (1.0f));
    fb.items.add (juce::FlexItem (rightPanel).withFlex (1.0f));

    fb.performLayout (getLocalBounds().reduced (2.0f));
}

void ToolBox::timerCallback ()
{
    if (parent == nullptr)
        return;

    const auto pos = parent->getScreenPosition();
    const auto height = parent->getHeight();
    if (pos != parentPos || height != parentHeight)
    {
        const auto width = 420;
        parentPos = pos;
        parentHeight = height;
        setBounds (parentPos.getX() - width, parentPos.getY(),
                   width,                    parentHeight * 0.9f);
    }
}

juce::File ToolBox::getLastLocation() const
{
    if (lastLocation.exists())
        return lastLocation;

    auto start = juce::File::getSpecialLocation (juce::File::currentExecutableFile);
    while (start.exists() && start.getFileName() != "Builds")
        start = start.getParentDirectory();

    if (start.getFileName() == "Builds")
    {
        auto resources = start.getSiblingFile ("Resources");
        if (resources.isDirectory())
            return resources;

        auto sources = start.getSiblingFile ("Sources");
        if (sources.isDirectory())
            return sources;
    }

    return juce::File::getSpecialLocation (juce::File::currentExecutableFile);
}




} // namespace foleys
