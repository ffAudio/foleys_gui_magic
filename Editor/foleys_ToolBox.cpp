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

namespace IDs
{
    static juce::String lastLocation { "lastLocation" };
}

ToolBox::ToolBox (juce::Component* parentToUse, MagicGUIBuilder& builderToControl)
  : parent (parentToUse),
    builder (builderToControl),
    undo (builder.getUndoManager())
{
    EditorColours::background = findColour (juce::ResizableWindow::backgroundColourId);
    EditorColours::outline = juce::Colours::silver;
    EditorColours::text = juce::Colours::white;
    EditorColours::disabledText = juce::Colours::grey;
    EditorColours::removeButton = juce::Colours::darkred;
    EditorColours::selectedBackground = juce::Colours::darkorange;

    setOpaque (true);
    setWantsKeyboardFocus (true);

    fileMenu.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);
    viewMenu.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);
    undoButton.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);
    editSwitch.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);

    addAndMakeVisible (fileMenu);
    addAndMakeVisible (viewMenu);
    addAndMakeVisible (undoButton);
    addAndMakeVisible (editSwitch);

    fileMenu.onClick = [&]
    {
        juce::PopupMenu file;
        file.addItem ("Load XML", [&] { loadDialog(); });
        file.addItem ("Save XML", [&] { saveDialog(); });
        file.addSeparator();
        file.addItem ("Clear",    [&] { builder.clearGUI(); });
        file.addItem ("Default",  [&] { builder.createDefaultGUITree (false); });
        file.show();
    };

    viewMenu.onClick = [&]
    {
        juce::PopupMenu view;

        view.addItem ("Left",  true, positionOption == left, [&]() { setToolboxPosition (left); });
        view.addItem ("Right", true, positionOption == right, [&]() { setToolboxPosition (right); });
        view.addItem ("Detached", true, positionOption == detached, [&]() { setToolboxPosition (detached); });
        view.addSeparator();
        view.addItem ("AlwaysOnTop", true, isAlwaysOnTop(), [&]() { setAlwaysOnTop ( ! isAlwaysOnTop() ); });

        view.show ();
    };

    undoButton.onClick = [&]
    {
        undo.undo();
    };

    editSwitch.setClickingTogglesState (true);
    editSwitch.setColour (juce::TextButton::buttonOnColourId, EditorColours::selectedBackground);
    editSwitch.onStateChange = [&]
    {
        builder.setEditMode (editSwitch.getToggleState());
    };

    addAndMakeVisible (treeEditor);
    addAndMakeVisible (resizer1);
    addAndMakeVisible (propertiesEditor);
    addAndMakeVisible (resizer3);
    addAndMakeVisible (palette);

    resizeManager.setItemLayout (0, 1, -1.0, -0.4);
    resizeManager.setItemLayout (1, 6, 6, 6);
    resizeManager.setItemLayout (2, 1, -1.0, -0.3);
    resizeManager.setItemLayout (3, 6, 6, 6);
    resizeManager.setItemLayout (4, 1, -1.0, -0.3);

    addChildComponent (resizeCorner);
    resizeCorner.setAlwaysOnTop (true);

    setBounds (100, 100, 300, 700);
    addToDesktop (getLookAndFeel().getMenuWindowFlags());

    startTimer (100);

    setVisible (true);

    stateWasReloaded();

    parent->addKeyListener (this);
}

ToolBox::~ToolBox()
{
    if (parent != nullptr)
        parent->removeKeyListener (this);
}

void ToolBox::mouseDown (const juce::MouseEvent& e)
{
    if (positionOption == PositionOption::detached)
        componentDragger.startDraggingComponent (this, e);
}

void ToolBox::mouseDrag (const juce::MouseEvent& e)
{
    if (positionOption == PositionOption::detached)
        componentDragger.dragComponent (this, e, nullptr);
}

void ToolBox::loadDialog()
{
    auto dialog = std::make_unique<FileBrowserDialog>(NEEDS_TRANS ("Cancel"), NEEDS_TRANS ("Load"),
                                                      juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                                                      getLastLocation(), getFileFilter());
    dialog->setAcceptFunction ([&, dlg=dialog.get()]
    {
        loadGUI (dlg->getFile());
        builder.closeOverlayDialog();
    });
    dialog->setCancelFunction ([&]
    {
        builder.closeOverlayDialog();
    });

    builder.showOverlayDialog (std::move (dialog));
}

void ToolBox::saveDialog()
{
    auto dialog = std::make_unique<FileBrowserDialog>(NEEDS_TRANS ("Cancel"), NEEDS_TRANS ("Save"),
                                                      juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting,
                                                      getLastLocation(), getFileFilter());
    dialog->setAcceptFunction ([&, dlg=dialog.get()]
    {
        saveGUI (dlg->getFile());
        builder.closeOverlayDialog();
    });
    dialog->setCancelFunction ([&]
    {
        builder.closeOverlayDialog();
    });

    builder.showOverlayDialog (std::move (dialog));
}

void ToolBox::loadGUI (const juce::File& xmlFile)
{
    juce::FileInputStream stream (xmlFile);
    auto tree = juce::ValueTree::fromXml (stream.readEntireStreamAsString());

    if (tree.isValid() && tree.getType() == IDs::magic)
    {
        builder.setConfigTree (tree);
        stateWasReloaded();
    }

    setLastLocation (xmlFile);
}

void ToolBox::saveGUI (const juce::File& xmlFile)
{
    juce::FileOutputStream stream (xmlFile);
    stream.setPosition (0);
    stream.truncate();
    stream.writeString (builder.getConfigTree().toXmlString());
    setLastLocation (xmlFile);
}

void ToolBox::setSelectedNode (const juce::ValueTree& node)
{
    treeEditor.setSelectedNode (node);
    propertiesEditor.setNodeToEdit (node);
    builder.setSelectedNode (node);
}

void ToolBox::setNodeToEdit (juce::ValueTree node)
{
    propertiesEditor.setNodeToEdit (node);
}

void ToolBox::stateWasReloaded()
{
    treeEditor.updateTree();
    propertiesEditor.setStyle (builder.getStylesheet().getCurrentStyle());
    palette.update();
}

void ToolBox::paint (juce::Graphics& g)
{
    g.fillAll (EditorColours::background);
    g.setColour (EditorColours::outline);
    g.drawRect (getLocalBounds().toFloat(), 2.0f);
    g.setColour (EditorColours::text);
    g.drawFittedText ("foleys GUI magic", getLocalBounds().withHeight (24), juce::Justification::centred, 1);
}

void ToolBox::resized()
{
    auto bounds = getLocalBounds().reduced (2).withTop (24);
    auto buttons = bounds.removeFromTop (24);
    auto w = buttons.getWidth() / 5;
    fileMenu.setBounds (buttons.removeFromLeft (w));
    viewMenu.setBounds (buttons.removeFromLeft (w));
    undoButton.setBounds (buttons.removeFromLeft (w));
    editSwitch.setBounds (buttons.removeFromLeft (w));

    juce::Component* comps[] = {
        &treeEditor,
        &resizer1,
        &propertiesEditor,
        &resizer3,
        &palette
    };

    resizeManager.layOutComponents (comps, 5,
                                    bounds.getX(),
                                    bounds.getY(),
                                    bounds.getWidth(),
                                    bounds.getHeight(),
                                    true, true);

    const int resizeCornerSize { 20 };
    const auto bottomRight { getLocalBounds().getBottomRight() };

    juce::Rectangle<int> resizeCornerArea { bottomRight.getX() - resizeCornerSize,
                                            bottomRight.getY() - resizeCornerSize,
                                            resizeCornerSize,
                                            resizeCornerSize };
    resizeCorner.setBounds (resizeCornerArea);
}

bool ToolBox::keyPressed (const juce::KeyPress& key, juce::Component*)
{
    return keyPressed (key);
}

bool ToolBox::keyPressed (const juce::KeyPress& key)
{
    if (key.isKeyCode (juce::KeyPress::backspaceKey) || key.isKeyCode (juce::KeyPress::deleteKey))
    {
        auto selected = builder.getSelectedNode();
        if (selected.isValid())
        {
            auto p = selected.getParent();
            if (p.isValid())
                p.removeChild (selected, &undo);
        }

        return true;
    }

    if (key.isKeyCode ('Z') && key.getModifiers().isCommandDown())
    {
        if (key.getModifiers().isShiftDown())
            undo.redo();
        else
            undo.undo();

        return true;
    }

    if (key.isKeyCode ('C') && key.getModifiers().isCommandDown())
    {
        auto selected = builder.getSelectedNode();
        if (selected.isValid())
            juce::SystemClipboard::copyTextToClipboard (selected.toXmlString());

        return true;
    }

    if (key.isKeyCode ('V') && key.getModifiers().isCommandDown())
    {
        auto paste = juce::ValueTree::fromXml (juce::SystemClipboard::getTextFromClipboard());
        auto selected = builder.getSelectedNode();
        if (paste.isValid() && selected.isValid())
            builder.draggedItemOnto (paste, selected);

        return true;
    }

    return false;
}

void ToolBox::timerCallback ()
{
    updateToolboxPosition();
}

void ToolBox::setToolboxPosition (PositionOption position)
{
    positionOption = position;
    const auto isDetached = (positionOption == PositionOption::detached);

    resizeCorner.setVisible (isDetached);

    if (isDetached)
        stopTimer ();
    else
        startTimer (100);
}

void ToolBox::updateToolboxPosition()
{
    if (parent == nullptr || positionOption == PositionOption::detached)
        return;

    const auto parentBounds = parent->getScreenBounds();
    const auto width { 280 };
    const auto height = juce::roundToInt (parentBounds.getHeight() * 0.9f);

    if (positionOption == PositionOption::left)
        setBounds (parentBounds.getX() - width, parentBounds.getY(), width, height);
    else if (positionOption == PositionOption::right)
        setBounds (parentBounds.getRight(), parentBounds.getY(), width, height);
}

juce::File ToolBox::getLastLocation() const
{
    juce::File lastLocation;

    juce::ApplicationProperties properties;
    properties.setStorageParameters (ToolBox::getApplicationPropertyStorage());
    if (auto* p = properties.getUserSettings())
        lastLocation = juce::File (p->getValue (IDs::lastLocation));

    if (lastLocation.exists())
        return lastLocation;

    auto start = juce::File::getSpecialLocation (juce::File::currentExecutableFile);
    while (start.exists() && !start.isRoot() && start.getFileName() != "Builds")
        start = start.getParentDirectory();

    if (start.getFileName() == "Builds")
    {
        auto resources = start.getSiblingFile ("Resources");
        if (resources.isDirectory())
            return resources;

        auto sources = start.getSiblingFile ("Source");
        if (sources.isDirectory())
            return sources;
    }

    return juce::File::getSpecialLocation (juce::File::currentExecutableFile);
}

void ToolBox::setLastLocation(juce::File file)
{
    juce::ApplicationProperties properties;
    properties.setStorageParameters (ToolBox::getApplicationPropertyStorage());
    if (auto* p = properties.getUserSettings())
        p->setValue (IDs::lastLocation, file.getFullPathName());
}

std::unique_ptr<juce::FileFilter> ToolBox::getFileFilter() const
{
    return std::make_unique<juce::WildcardFileFilter>("*.xml", "*", "XML files");
}

juce::PropertiesFile::Options ToolBox::getApplicationPropertyStorage()
{
    juce::PropertiesFile::Options options;
    options.folderName      = "FoleysFinest";
    options.applicationName = "foleys_gui_magic";
    options.filenameSuffix  = ".settings";
    options.osxLibrarySubFolder = "Application Support";
    return options;
}

} // namespace foleys
