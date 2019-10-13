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


MagicBuilder::MagicBuilder (juce::Component& parentToUse)
  : parent (parentToUse)
{
#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    juce::MessageManager::callAsync ([&]
                                     {
                                         magicToolBox = std::make_unique<ToolBox>(parent.getTopLevelComponent(), *this);
                                     });
#endif
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
    updateLayout();

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

void MagicBuilder::restoreGUI (const juce::ValueTree& gui)
{
    if (gui.isValid() == false)
        return;

    config = gui;

    updateAll();

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    if (magicToolBox.get() != nullptr)
        magicToolBox->stateWasReloaded();
#endif
}

void MagicBuilder::updateComponents()
{
    createDefaultGUITree (true);

    auto rootNode = config.getOrCreateChildWithName (IDs::view, &undo);
    root = restoreNode (parent, rootNode);

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    if (root.get() != nullptr)
        root->setEditMode (editMode);
#endif
}

void MagicBuilder::updateLayout()
{
    if (root.get() != nullptr)
        root->setBounds (parent.getLocalBounds());
}

void MagicBuilder::registerLookAndFeel (juce::String name, std::unique_ptr<juce::LookAndFeel> lookAndFeel)
{
    stylesheet.registerLookAndFeel (name, std::move (lookAndFeel));
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

juce::StringArray MagicBuilder::getAllLayoutPropertyNames() const
{
    return
    {
        IDs::id.toString(),
        IDs::styleClass.toString(),
        IDs::lookAndFeel.toString(),
        IDs::margin.toString(),
        IDs::padding.toString(),
        IDs::border.toString(),
        IDs::backgroundImage.toString(),
        IDs::caption.toString(),
        IDs::captionSize.toString(),
        IDs::captionPlacement.toString(),
        IDs::display.toString(),
        IDs::flexDirection.toString(),
        IDs::flexGrow.toString(),
        IDs::flexShrink.toString(),
        IDs::flexWrap.toString(),
        IDs::flexOrder.toString(),
        IDs::flexAlignContent.toString(),
        IDs::flexAlignItems.toString(),
        IDs::flexJustifyContent.toString(),
        IDs::flexAlignSelf.toString(),
        IDs::minWidth.toString(),
        IDs::maxWidth.toString(),
        IDs::minHeight.toString(),
        IDs::maxHeight.toString(),
        IDs::width.toString(),
        IDs::height.toString()
    };
}

void MagicBuilder::addSettableProperty (juce::Identifier type, SettableProperty property)
{
    const auto& it = settableProperties.find (type);
    if (it == settableProperties.end())
        settableProperties [type] = { property };
    else
        it->second.push_back (property);
}

std::vector<SettableProperty> MagicBuilder::getSettableProperties (juce::Identifier type) const
{
    const auto& it = settableProperties.find (type);
    if (it != settableProperties.end())
        return it->second;

    return {};
}


#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
void MagicBuilder::setEditMode (bool shouldEdit)
{
    editMode = shouldEdit;

    if (root.get() != nullptr)
        root->setEditMode (shouldEdit);

    if (shouldEdit == false)
        setSelectedNode (juce::ValueTree());

    parent.repaint();
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

        parent.repaint();
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

void MagicBuilder::valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&)
{
    updateAll();
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


template <class AppType>
MagicGUIBuilder<AppType>::MagicGUIBuilder (juce::Component& parentToUse, AppType& appToUse, MagicProcessorState* magicStateToUse)
  : MagicBuilder (parentToUse),
    app (appToUse),
    magicState (magicStateToUse)
{
    config = juce::ValueTree (IDs::magic);

    updateStylesheet();
}

template <class AppType>
void MagicGUIBuilder<AppType>::registerFactory (juce::Identifier type, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&, AppType&)> factory)
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

template <class AppType>
juce::StringArray MagicGUIBuilder<AppType>::getFactoryNames() const
{
    juce::StringArray names { IDs::view.toString() };

    names.ensureStorageAllocated (int (factories.size()));
    for (const auto& f : factories)
        names.add (f.first.toString());

    return names;
}

template <class AppType>
juce::StringArray MagicGUIBuilder<AppType>::getParameterNames() const
{
    if (magicState == nullptr)
        return {};

    return magicState->getParameterNames();
}

template <class AppType>
juce::StringArray MagicGUIBuilder<AppType>::getPlotSourcesNames() const
{
    if (magicState == nullptr)
        return {};

    return magicState->getPlotSourcesNames();
}


template <class AppType>
std::unique_ptr<Decorator> MagicGUIBuilder<AppType>::restoreNode (juce::Component& component, const juce::ValueTree& node)
{
    if (node.getType() == IDs::view)
    {
        auto item = std::make_unique<Container>(*this, node);
        for (auto childNode : node)
        {
            item->addChildItem (restoreNode (*item, childNode));
        }

        item->configureDecorator (stylesheet, node);

        auto display = stylesheet.getProperty (IDs::display, node).toString();
        if (display == IDs::contents)
        {
            item->layout = Container::Layout::Contents;
        }
        else
        {
            item->layout = Container::Layout::FlexBox;
            stylesheet.configureFlexBox (item->flexBox, node);
        }

        component.addAndMakeVisible (item.get());
        return item;
    }

    auto factory = factories [node.getType()];

    if (factory == nullptr)
    {
        DBG ("No factory for: " << node.getType().toString());
    }

    auto item = std::make_unique<Decorator> (*this, node, factory ? factory (node, app) : nullptr);
    component.addAndMakeVisible (item.get());

    item->configureDecorator (stylesheet, node);

    item->configureComponent (stylesheet, node);

    stylesheet.configureFlexBoxItem (item->flexItem, node);

    const auto translation = colourTranslations.find (node.getType());
    if (translation != colourTranslations.end() && item->getWrappedComponent() != nullptr)
    {
        for (auto& pair : translation->second)
        {
            auto colour = stylesheet.getProperty (pair.first, node).toString();
            if (colour.isNotEmpty())
                item->getWrappedComponent()->setColour (pair.second, stylesheet.parseColour (colour));
        }
    }

    if (magicState != nullptr)
    {
        auto parameter = node.getProperty (IDs::parameter, juce::String()).toString();
        if (parameter.isNotEmpty())
            item->connectToState (parameter, magicState->getValueTreeState());
    }

    return item;
}

template <>
void MagicGUIBuilder<juce::AudioProcessor>::createDefaultFromParameters (juce::ValueTree& node, const juce::AudioProcessorParameterGroup& tree)
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

template <>
void MagicGUIBuilder<juce::AudioProcessor>::createDefaultGUITree (bool keepExisting)
{
    if (keepExisting && config.getChildWithName (IDs::view).isValid())
        return;

    auto rootNode = config.getOrCreateChildWithName (IDs::view, &undo);
    rootNode.setProperty (IDs::id, IDs::root, &undo);

    auto current = rootNode;

    if (magicState != nullptr)
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


} // namespace foleys
