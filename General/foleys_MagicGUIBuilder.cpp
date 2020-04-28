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


MagicGUIBuilder::MagicGUIBuilder (MagicProcessorState* state)
  : magicState (state)
{
    config = juce::ValueTree (IDs::magic);

    updateStylesheet();
}

MagicGUIBuilder::~MagicGUIBuilder()
{
    config.removeListener (this);
}

Stylesheet& MagicGUIBuilder::getStylesheet()
{
    return stylesheet;
}

juce::ValueTree& MagicGUIBuilder::getConfigTree()
{
    return config;
}

juce::ValueTree MagicGUIBuilder::getGuiRootNode()
{
    return config.getChildWithName (IDs::view);
}

void MagicGUIBuilder::updateAll()
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

void MagicGUIBuilder::updateStylesheet()
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

    stylesheet.updateStyleClasses (*this);
    stylesheet.updateValidRanges();
}

void MagicGUIBuilder::clearGUI()
{
    auto guiNode = config.getOrCreateChildWithName (IDs::view, &undo);
    guiNode.removeAllChildren (&undo);
    guiNode.removeAllProperties (&undo);
}

void MagicGUIBuilder::showOverlayDialog (std::unique_ptr<juce::Component> dialog)
{
    if (parent == nullptr)
        return;

    overlayDialog = std::move (dialog);
    parent->addAndMakeVisible (overlayDialog.get());

    updateLayout();
}

void MagicGUIBuilder::closeOverlayDialog()
{
    overlayDialog.reset();
}

void MagicGUIBuilder::setConfigTree (const juce::ValueTree& gui)
{
    if (gui.isValid() == false)
        return;

    if (config.isValid())
    {
        auto parentNode = config.getParent();
        parentNode.removeChild (config, nullptr);
        config = gui;
        if (parentNode.isValid())
            parentNode.appendChild (config, nullptr);
    }
    else
    {
        config = gui;
    }

    undo.clearUndoHistory();
    updateAll();
}

void MagicGUIBuilder::restoreGUI (juce::Component& parentToUse)
{
    parent = &parentToUse;

    updateAll();

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    if (magicToolBox.get() != nullptr)
        magicToolBox->stateWasReloaded();
#endif
}

void MagicGUIBuilder::updateComponents()
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

void MagicGUIBuilder::updateProperties (Decorator& item)
{
    const auto& configNode = item.getConfigNode();

    item.configureDecorator (stylesheet, configNode);
    item.configureComponent (stylesheet, configNode);
    item.configureFlexBoxItem (configNode);

    const auto translation = colourTranslations.find (configNode.getType());
    if (translation != colourTranslations.end() && item.getWrappedComponent() != nullptr)
    {
        for (auto& pair : translation->second)
        {
            auto colour = getStyleProperty (pair.first, configNode).toString();
            if (colour.isNotEmpty())
                item.getWrappedComponent()->setColour (pair.second, Stylesheet::parseColour (colour));
        }
    }

    if (auto* container = dynamic_cast<Container*>(&item))
    {
        container->configureFlexBox (configNode);

        for (auto& child : *container)
            updateProperties (*child);

        const auto display = getStyleProperty (IDs::display, configNode).toString();
        if (display == IDs::contents)
            container->setLayoutMode (Container::Layout::Contents);
        else if (display == IDs::tabbed)
            container->setLayoutMode (Container::Layout::Tabbed);
        else
            container->setLayoutMode (Container::Layout::FlexBox);

        auto repaintHz = getStyleProperty (IDs::repaintHz, configNode).toString();
        if (repaintHz.isNotEmpty())
            container->setRefreshRate (repaintHz.getIntValue());
    }
}

void MagicGUIBuilder::updateLayout()
{
    if (parent == nullptr)
        return;

    if (root.get() != nullptr)
    {
        if (! stylesheet.setMediaSize (parent->getWidth(), parent->getHeight()))
        {
            stylesheet.updateValidRanges();
            updateProperties (*root);
        }

        if (root->getBounds() == parent->getLocalBounds())
            root->updateLayout();
        else
            root->setBounds (parent->getLocalBounds());
    }

    if (overlayDialog)
    {
        if (overlayDialog->getBounds() == parent->getLocalBounds())
            overlayDialog->resized();
        else
            overlayDialog->setBounds (parent->getLocalBounds());
    }

    parent->repaint();
}

void MagicGUIBuilder::registerFactory (juce::Identifier type, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&)> factory)
{
    if (factories.find (type) != factories.cend())
    {
        // You tried to add two factories with the same type name!
        // That cannot work, the second factory will be ignored.
        jassertfalse;
        return;
    }

    factories [type] = factory;
}

juce::StringArray MagicGUIBuilder::getFactoryNames() const
{
    juce::StringArray names { IDs::view.toString() };

    names.ensureStorageAllocated (int (factories.size()));
    for (const auto& f : factories)
        names.add (f.first.toString());

    return names;
}

void MagicGUIBuilder::registerLookAndFeel (juce::String name, std::unique_ptr<juce::LookAndFeel> lookAndFeel)
{
    stylesheet.registerLookAndFeel (name, std::move (lookAndFeel));
}

void MagicGUIBuilder::registerJUCELookAndFeels()
{
    stylesheet.registerLookAndFeel ("LookAndFeel_V1", std::make_unique<juce::LookAndFeel_V1>());
    stylesheet.registerLookAndFeel ("LookAndFeel_V2", std::make_unique<juce::LookAndFeel_V2>());
    stylesheet.registerLookAndFeel ("LookAndFeel_V3", std::make_unique<juce::LookAndFeel_V3>());
    stylesheet.registerLookAndFeel ("LookAndFeel_V4", std::make_unique<juce::LookAndFeel_V4>());
    stylesheet.registerLookAndFeel ("FoleysFinest", std::make_unique<LookAndFeel>());
    stylesheet.registerLookAndFeel ("Skeuomorphic", std::make_unique<Skeuomorphic>());
}

void MagicGUIBuilder::setColourTranslation (juce::Identifier type, std::vector<std::pair<juce::String, int>> mapping)
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

juce::var MagicGUIBuilder::getStyleProperty (const juce::Identifier& name, const juce::ValueTree& node) const
{
    return stylesheet.getStyleProperty (name, node);
}

int MagicGUIBuilder::findColourId (juce::Identifier type, juce::Identifier name)
{
    const auto& map = colourTranslations.find (type);
    if (map == colourTranslations.end())
        return -1;

    for (const auto& pair : map->second)
        if (pair.first == name.toString())
            return pair.second;

    return -1;
}

int MagicGUIBuilder::findColourId (juce::Identifier name)
{
    for (const auto& map : colourTranslations)
    {
        auto id = findColourId (map.first, name);
        if (id >= 0)
            return id;
    }

    return -1;
}

void MagicGUIBuilder::removeStyleClassReferences (juce::ValueTree tree, const juce::String& name)
{
    if (tree.hasProperty (IDs::styleClass))
    {
        const auto separator = " ";
        auto strings = juce::StringArray::fromTokens (tree.getProperty (IDs::styleClass).toString(), separator, "");
        strings.removeEmptyStrings (true);
        strings.removeString (name);
        tree.setProperty (IDs::styleClass, strings.joinIntoString (separator), &undo);
    }

    for (auto child : tree)
        removeStyleClassReferences (child, name);
}

juce::StringArray MagicGUIBuilder::getColourNames (juce::Identifier type) const
{
    juce::StringArray names;

    const auto& it = colourTranslations.find (type);
    if (it != colourTranslations.end())
        for (const auto& pair : it->second)
            names.addIfNotAlreadyThere (pair.first);

    return names;
}

juce::StringArray MagicGUIBuilder::getAllColourNames() const
{
    juce::StringArray names { IDs::backgroundColour.toString(), IDs::borderColour.toString(), IDs::captionColour.toString() };

    for (const auto& table : colourTranslations)
        for (const auto& pair : table.second)
            names.addIfNotAlreadyThere (pair.first);

    return names;
}

void MagicGUIBuilder::addSettableProperty (juce::Identifier type, std::unique_ptr<SettableProperty> property)
{
    settableProperties [type].push_back (std::move (property));
}

const std::vector<std::unique_ptr<SettableProperty>>& MagicGUIBuilder::getSettableProperties (juce::Identifier type) const
{
    const auto& it = settableProperties.find (type);
    if (it != settableProperties.end())
        return it->second;

    return emptyPropertyList;
}

std::unique_ptr<Decorator> MagicGUIBuilder::restoreNode (juce::Component& component, const juce::ValueTree& node)
{
    if (node.getType() == IDs::view)
    {
        auto item = std::make_unique<Container>(*this, node);
        for (auto childNode : node)
            item->addChildItem (restoreNode (*item, childNode));

        component.addAndMakeVisible (item.get());

        item->updateContinuousRedraw();

        // Xcode 8 needs the move for returning a derrived class
        return std::move (item);
    }

    auto factory = factories [node.getType()];

    if (factory == nullptr)
    {
        DBG ("No factory for: " << node.getType().toString());
    }

    auto item = std::make_unique<Decorator> (*this, node, factory ? factory (node) : nullptr);
    component.addAndMakeVisible (item.get());

    return item;
}

void MagicGUIBuilder::populateSettableOptionsMenu (juce::ComboBox& comboBox, SettableProperty::PropertyType type) const
{
    if (magicState)
        return magicState->populateSettableOptionsMenu (comboBox, type);
}

juce::var MagicGUIBuilder::getPropertyDefaultValue (juce::Identifier type, juce::Identifier property) const
{
    const auto& properties = getSettableProperties (type);

    const auto& settableProperty = std::find_if (properties.begin(), properties.end(), [property] (const auto& element) { return element->name == property; });
    if (settableProperty != properties.end())
        return settableProperty->get()->defaultValue;

    // flexbox
    if (property == IDs::flexDirection) return IDs::flexDirRow;
    if (property == IDs::flexWrap)      return IDs::flexNoWrap;
    if (property == IDs::flexAlignContent) return IDs::flexStretch;
    if (property == IDs::flexAlignItems) return IDs::flexStretch;
    if (property == IDs::flexJustifyContent) return IDs::flexStart;
    if (property == IDs::flexAlignSelf) return IDs::flexStretch;
    if (property == IDs::flexOrder) return 0;
    if (property == IDs::flexGrow) return 1.0;
    if (property == IDs::flexShrink) return 1.0;
    if (property == IDs::minWidth) return 0.0;
    if (property == IDs::minHeight) return 0.0;
    if (property == IDs::display) return IDs::flexbox;

    return {};
}

juce::NamedValueSet MagicGUIBuilder::makeJustificationsChoices()
{
    juce::NamedValueSet choices;
    choices.set ("centred",         juce::Justification::centred);
    choices.set ("top-left",        juce::Justification::topLeft);
    choices.set ("centred-left",    juce::Justification::centredLeft);
    choices.set ("bottom-left",     juce::Justification::bottomLeft);
    choices.set ("centred-top",     juce::Justification::centredTop);
    choices.set ("centred-bottom",  juce::Justification::centredBottom);
    choices.set ("top-right",       juce::Justification::topRight);
    choices.set ("centred-right",   juce::Justification::centredRight);
    choices.set ("bottom-right",    juce::Justification::bottomRight);
    return choices;
}

void MagicGUIBuilder::changeListenerCallback (juce::ChangeBroadcaster*)
{
    if (root.get() != nullptr)
        updateProperties (*root);

    updateLayout();
}

MagicProcessorState* MagicGUIBuilder::getProcessorState()
{
    return magicState;
}

juce::UndoManager& MagicGUIBuilder::getUndoManager()
{
    return undo;
}

void MagicGUIBuilder::createDefaultFromParameters (juce::ValueTree& node, const juce::AudioProcessorParameterGroup& tree)
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

void MagicGUIBuilder::createDefaultGUITree (bool keepExisting)
{
    if (keepExisting && config.getChildWithName (IDs::view).isValid())
        return;

    auto rootNode = config.getOrCreateChildWithName (IDs::view, &undo);
    rootNode.setProperty (IDs::id, IDs::root, &undo);

    auto current = rootNode;

    if (magicState)
    {
        auto plotNames = magicState->getPlotSourcesNames();

        if (plotNames.isEmpty() == false)
        {
            juce::StringArray colours { "orange", "blue", "red", "silver", "green", "cyan", "brown", "white" };
            int nextColour = 0;

            auto child = juce::ValueTree (IDs::view, {
                { IDs::id, "plot-view" },
                { IDs::styleClass, "plot-view" }});

            for (auto plotName : plotNames)
            {
                child.appendChild ({IDs::plot, {
                    { IDs::source, plotName },
                    { "plot-color", colours [nextColour++] }}}, nullptr);

                if (nextColour >= colours.size())
                    nextColour = 0;
            }

            current.appendChild (child, nullptr);

            juce::ValueTree parameters { IDs::view, {
                { IDs::styleClass, "parameters nomargin" }}};

            current.appendChild (parameters, nullptr);
            current = parameters;
        }

        createDefaultFromParameters (current, magicState->getProcessor().getParameterTree());
    }
}

void MagicGUIBuilder::valueTreePropertyChanged (juce::ValueTree& node, const juce::Identifier&)
{
    if (node.isAChildOf (stylesheet.getCurrentStyle()))
        stylesheet.updateStyleClasses (*this);

    if (root)
    {
        stylesheet.updateValidRanges();
        updateProperties (*root);
    }

    updateLayout();
}

void MagicGUIBuilder::valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&)
{
    updateAll();
}

void MagicGUIBuilder::valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int)
{
    updateAll();
}

void MagicGUIBuilder::valueTreeChildOrderChanged (juce::ValueTree&, int, int)
{
    updateAll();
}

void MagicGUIBuilder::valueTreeParentChanged (juce::ValueTree&)
{
    updateAll();
}

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE

void MagicGUIBuilder::attachToolboxToWindow (juce::Component& window)
{
    juce::Component::SafePointer<juce::Component> reference (&window);

    juce::MessageManager::callAsync ([&, reference]
                                     {
                                         if (reference != nullptr)
                                             magicToolBox = std::make_unique<ToolBox>(reference->getTopLevelComponent(), *this);
                                     });
}

void MagicGUIBuilder::setEditMode (bool shouldEdit)
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

bool MagicGUIBuilder::isEditModeOn() const
{
    return editMode;
}

void MagicGUIBuilder::setSelectedNode (const juce::ValueTree& node)
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

const juce::ValueTree& MagicGUIBuilder::getSelectedNode() const
{
    return selectedNode;
}

void MagicGUIBuilder::draggedItemOnto (juce::ValueTree dragged, juce::ValueTree target, int index)
{
    if (dragged == target)
        return;

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

ToolBox& MagicGUIBuilder::getMagicToolBox()
{
    // The magicToolBox should always be present!
    // This method wouldn't be included, if
    // FOLEYS_SHOW_GUI_EDITOR_PALLETTE was 0
    jassert (magicToolBox.get() != nullptr);

    return *magicToolBox;
}

#endif


} // namespace foleys
