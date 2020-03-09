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


MagicBuilder::MagicBuilder()
{
}

MagicBuilder::~MagicBuilder()
{
    config.removeListener (this);
}

Stylesheet& MagicBuilder::getStylesheet()
{
    return stylesheet;
}

juce::ValueTree& MagicBuilder::getConfigTree()
{
    return config;
}

juce::ValueTree MagicBuilder::getGuiRootNode()
{
    return config.getChildWithName (IDs::view);
}

void MagicBuilder::updateAll()
{
    updateStylesheet();
    updateComponents();

    if (root)
        updateProperties (*root);

    updateLayout();

    if (root)
        updateProperties (*root);

    config.addListener (this);
}

void MagicBuilder::updateStylesheet()
{
    auto stylesNode = config.getOrCreateChildWithName (IDs::styles, &undo);
    if (stylesNode.getNumChildren() == 0)
        stylesNode.appendChild (Stylesheet::createDefaultStyle(), &undo);

    auto selectedName = stylesNode.getProperty (IDs::selected, {}).toString();
    if (selectedName.isNotEmpty())
    {
        auto style = stylesNode.getChildWithProperty (IDs::name, selectedName);
        stylesheet.setStyle (style);
    }
    else
    {
        stylesheet.setStyle (stylesNode.getChild (0));
    }
}

void MagicBuilder::clearGUI()
{
    auto guiNode = config.getOrCreateChildWithName (IDs::view, &undo);
    guiNode.removeAllChildren (&undo);
    guiNode.removeAllProperties (&undo);
}

void MagicBuilder::showOverlayDialog (std::unique_ptr<juce::Component> dialog)
{
    if (parent == nullptr)
        return;

    overlayDialog = std::move (dialog);
    parent->addAndMakeVisible (overlayDialog.get());

    updateLayout();
}

void MagicBuilder::closeOverlayDialog()
{
    overlayDialog.reset();
}

void MagicBuilder::setConfigTree (const juce::ValueTree& gui)
{
    if (gui.isValid() == false)
        return;

    config = gui;
    undo.clearUndoHistory();
    updateAll();
}

void MagicBuilder::restoreGUI (juce::Component& parentToUse)
{
    parent = &parentToUse;

    updateAll();

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    if (magicToolBox.get() != nullptr)
        magicToolBox->stateWasReloaded();
#endif
}

void MagicBuilder::updateComponents()
{
    if (parent == nullptr)
        return;

    createDefaultGUITree (true);

    auto rootNode = config.getOrCreateChildWithName (IDs::view, &undo);
    root = restoreNode (*parent, rootNode);

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    if (root.get() != nullptr)
        root->setEditMode (editMode);
#endif
}

void MagicBuilder::updateProperties (Decorator& item)
{
    const auto& configNode = item.getConfigNode();

    item.configureDecorator (stylesheet, configNode);

    item.configureComponent (stylesheet, configNode);

    stylesheet.configureFlexBoxItem (item.flexItem, configNode);

    const auto translation = colourTranslations.find (configNode.getType());
    if (translation != colourTranslations.end() && item.getWrappedComponent() != nullptr)
    {
        for (auto& pair : translation->second)
        {
            auto colour = stylesheet.getProperty (pair.first, configNode).toString();
            if (colour.isNotEmpty())
                item.getWrappedComponent()->setColour (pair.second, stylesheet.parseColour (colour));
        }
    }

    if (auto* container = dynamic_cast<Container*>(&item))
    {
        stylesheet.configureFlexBoxItem (item.flexItem, configNode);

        auto display = stylesheet.getProperty (IDs::display, configNode).toString();
        if (display == IDs::contents)
        {
            container->layout = Container::Layout::Contents;
        }
        else
        {
            container->layout = Container::Layout::FlexBox;
            stylesheet.configureFlexBox (container->flexBox, configNode);
        }

        auto throttle = stylesheet.getProperty (IDs::throttle, configNode).toString();
        if (throttle.isNotEmpty())
            container->setMaxFPSrate (throttle.getIntValue());

        for (auto& child : *container)
            updateProperties (*child);
    }
}

void MagicBuilder::updateLayout()
{
    if (parent == nullptr)
        return;

    if (root.get() != nullptr)
        root->setBounds (parent->getLocalBounds());

    if (overlayDialog)
        overlayDialog->setBounds (parent->getLocalBounds());

    parent->repaint();
}

void MagicBuilder::registerLookAndFeel (juce::String name, std::unique_ptr<juce::LookAndFeel> lookAndFeel)
{
    stylesheet.registerLookAndFeel (name, std::move (lookAndFeel));
}

void MagicBuilder::registerJUCELookAndFeels()
{
    stylesheet.registerLookAndFeel ("LookAndFeel_V1", std::make_unique<juce::LookAndFeel_V1>());
    stylesheet.registerLookAndFeel ("LookAndFeel_V2", std::make_unique<juce::LookAndFeel_V2>());
    stylesheet.registerLookAndFeel ("LookAndFeel_V3", std::make_unique<juce::LookAndFeel_V3>());
    stylesheet.registerLookAndFeel ("LookAndFeel_V4", std::make_unique<juce::LookAndFeel_V4>());
    stylesheet.registerLookAndFeel ("FoleysFinest", std::make_unique<LookAndFeel>());
    stylesheet.registerLookAndFeel ("Skeuomorphic", std::make_unique<Skeuomorphic>());
}

void MagicBuilder::setColourTranslation (juce::Identifier type, std::vector<std::pair<juce::String, int>> mapping)
{
    if (colourTranslations.find (type) != colourTranslations.cend())
    {
        // You tried to add two colour mappings for the same type name!
        // That cannot work, the second mapping will be ignored.
        jassertfalse;
        return;
    }

    colourTranslations [type] = mapping;
}

int MagicBuilder::findColourId (juce::Identifier type, juce::Identifier name)
{
    const auto& map = colourTranslations.find (type);
    if (map == colourTranslations.end())
        return -1;

    for (const auto& pair : map->second)
        if (pair.first == name.toString())
            return pair.second;

    return -1;
}

int MagicBuilder::findColourId (juce::Identifier name)
{
    for (const auto& map : colourTranslations)
    {
        auto id = findColourId (map.first, name);
        if (id >= 0)
            return id;
    }

    return -1;
}

juce::StringArray MagicBuilder::getColourNames (juce::Identifier type) const
{
    juce::StringArray names;

    const auto& it = colourTranslations.find (type);
    if (it != colourTranslations.end())
        for (const auto& pair : it->second)
            names.addIfNotAlreadyThere (pair.first);

    return names;
}

juce::StringArray MagicBuilder::getAllColourNames() const
{
    juce::StringArray names { IDs::backgroundColour.toString(), IDs::borderColour.toString(), IDs::captionColour.toString() };

    for (const auto& table : colourTranslations)
        for (const auto& pair : table.second)
            names.addIfNotAlreadyThere (pair.first);

    return names;
}

void MagicBuilder::addSettableProperty (juce::Identifier type, std::unique_ptr<SettableProperty> property)
{
    settableProperties [type].push_back (std::move (property));
}

const std::vector<std::unique_ptr<SettableProperty>>& MagicBuilder::getSettableProperties (juce::Identifier type) const
{
    const auto& it = settableProperties.find (type);
    if (it != settableProperties.end())
        return it->second;

    return emptyPropertyList;
}

void MagicBuilder::createDefaultFromParameters (juce::ValueTree& node, const juce::AudioProcessorParameterGroup& tree)
{
    for (const auto& sub : tree.getSubgroups (false))
    {
        auto child = juce::ValueTree (IDs::view, {
            {IDs::caption, sub->getName()},
            {IDs::styleClass, "group"}});

        createDefaultFromParameters (child, *sub);
        node.appendChild (child, nullptr);
    }

    for (const auto& param : tree.getParameters (false))
    {
        auto child = juce::ValueTree (IDs::slider);
        if (dynamic_cast<juce::AudioParameterChoice*>(param) != nullptr)
            child = juce::ValueTree (IDs::comboBox);
        else if (dynamic_cast<juce::AudioParameterBool*>(param) != nullptr)
            child = juce::ValueTree (IDs::toggleButton);

        child.setProperty (IDs::caption, param->getName (64), nullptr);
        if (const auto* parameterWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(param))
            child.setProperty (IDs::parameter, parameterWithID->paramID, nullptr);

        node.appendChild (child, nullptr);
    }
}

void MagicBuilder::valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&)
{
    if (root)
        updateProperties (*root);

    updateLayout();
}

void MagicBuilder::valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&)
{
    updateAll();
}

void MagicBuilder::valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int)
{
    updateAll();
}

void MagicBuilder::valueTreeChildOrderChanged (juce::ValueTree&, int, int)
{
    updateAll();
}

void MagicBuilder::valueTreeParentChanged (juce::ValueTree&)
{
    updateAll();
}

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE

void MagicBuilder::attachToolboxToWindow (juce::Component& window)
{
    juce::Component::SafePointer<juce::Component> reference (&window);

    juce::MessageManager::callAsync ([&, reference]
                                     {
                                         if (reference != nullptr)
                                             magicToolBox = std::make_unique<ToolBox>(reference->getTopLevelComponent(), *this);
                                     });
}

void MagicBuilder::setEditMode (bool shouldEdit)
{
    editMode = shouldEdit;

    if (parent == nullptr)
        return;

    if (root.get() != nullptr)
        root->setEditMode (shouldEdit);

    if (shouldEdit == false)
        setSelectedNode (juce::ValueTree());

    parent->repaint();
}

bool MagicBuilder::isEditModeOn() const
{
    return editMode;
}

void MagicBuilder::setSelectedNode (const juce::ValueTree& node)
{
    if (selectedNode != node)
    {
        selectedNode = node;
        if (magicToolBox.get() != nullptr)
            magicToolBox->setSelectedNode (selectedNode);

        if (parent != nullptr)
            parent->repaint();
    }
}

const juce::ValueTree& MagicBuilder::getSelectedNode() const
{
    return selectedNode;
}

void MagicBuilder::draggedItemOnto (juce::ValueTree dragged, juce::ValueTree target, int index)
{
    undo.beginNewTransaction();

    auto targetParent  = target.getParent();
    auto draggedParent = dragged.getParent();

    if (draggedParent.isValid())
        draggedParent.removeChild (dragged, &undo);

    if (targetParent.isValid() != false && index < 0)
        index = targetParent.indexOf (target);

    if (target.getType() == IDs::view)
        target.addChild (dragged, index, &undo);
    else
        targetParent.addChild (dragged, index, &undo);
}

ToolBox& MagicBuilder::getMagicToolBox()
{
    // The magicToolBox should always be present!
    // This method wouldn't be included, if
    // FOLEYS_SHOW_GUI_EDITOR_PALLETTE was 0
    jassert (magicToolBox.get() != nullptr);

    return *magicToolBox;
}

juce::UndoManager& MagicBuilder::getUndoManager()
{
    return undo;
}

#endif


} // namespace foleys
