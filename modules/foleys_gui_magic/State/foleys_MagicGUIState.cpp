/*
 ==============================================================================
    Copyright (c) 2019-2023 Foleys Finest Audio - Daniel Walz
    All rights reserved.

    **BSD 3-Clause License**

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

 ==============================================================================

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

#include "foleys_MagicGUIState.h"

namespace foleys
{

MagicGUIState::MagicGUIState()
{
}

MagicGUIState::~MagicGUIState()
{
    visualiserThread.stopThread (1000);
}

void MagicGUIState::addBackgroundProcessing (MagicPlotSource* source)
{
    if (auto* job = source->getBackgroundJob())
    {
        visualiserThread.addTimeSliceClient (job);
        visualiserThread.startThread();
    }
}

void MagicGUIState::addTrigger (const juce::Identifier& triggerID, std::function<void()> function)
{
    triggers [triggerID] = function;
}

std::function<void()> MagicGUIState::getTrigger (const juce::Identifier& triggerID)
{
    auto it = triggers.find (triggerID);
    if (it == triggers.end())
        return nullptr;

    return it->second;
}

juce::ValueTree MagicGUIState::getPropertyRoot()
{
    return state.getOrCreateChildWithName (IDs::properties, nullptr);
}

juce::ValueTree MagicGUIState::getPropertyRoot() const
{
    return state.getChildWithName (IDs::properties);
}

void MagicGUIState::setGuiValueTree (const juce::ValueTree& dom)
{
    jassert (dom.hasType (IDs::magic));

    guiValueTree = dom;
}

void MagicGUIState::setGuiValueTree (const char* data, int dataSize)
{
    juce::String text (data, size_t (dataSize));
    auto dom = juce::ValueTree::fromXml (text);
    if (dom.isValid())
        setGuiValueTree (dom);
}

void MagicGUIState::setGuiValueTree (const juce::File& file)
{
    auto dom = juce::ValueTree::fromXml (file.loadFileAsString());
    if (dom.isValid())
        setGuiValueTree (dom);
}

juce::ValueTree& MagicGUIState::getGuiTree()
{
    return guiValueTree;
}

juce::ValueTree& MagicGUIState::getValueTree()
{
    return state;
}

void MagicGUIState::setApplicationSettingsFile (juce::File file)
{
    settings->setFileName (file);
}

juce::ValueTree& MagicGUIState::getSettings()
{
    return settings->settings;
}

juce::Value MagicGUIState::getPropertyAsValue (const juce::String& pathToProperty)
{
    auto path = juce::StringArray::fromTokens (pathToProperty, ":", "");
    path.removeEmptyStrings();

    if (path.size() == 0)
        return {};

    auto tree = getPropertyRoot();

    for (int i = 0; i < path.size() - 1 && tree.isValid(); ++i)
        tree = tree.getOrCreateChildWithName (path [i], nullptr);

    auto propName = path [path.size()-1];
    if (!tree.hasProperty (propName))
        tree.setProperty (propName, {}, nullptr);

    return tree.getPropertyAsValue (propName, nullptr);
}

juce::StringArray MagicGUIState::getParameterNames() const
{
    return {};
}

void MagicGUIState::populatePropertiesMenu (juce::ComboBox& comboBox) const
{
    addPropertiesToMenu (getPropertyRoot(), comboBox, *comboBox.getRootMenu(), {});
}

juce::PopupMenu MagicGUIState::createPropertiesMenu (juce::ComboBox& combo) const
{
    juce::PopupMenu menu;
    addPropertiesToMenu (getPropertyRoot(), combo, menu, {});
    return menu;
}

juce::PopupMenu MagicGUIState::createTriggerMenu() const
{
    juce::PopupMenu menu;
    int index = 0;
    for (const auto& trigger : triggers)
        menu.addItem (++index, trigger.first.toString());

    return menu;
}

juce::PopupMenu MagicGUIState::createAssetFilesMenu() const
{
    juce::PopupMenu menu;
    int index = 0;
    for (const auto& name : Resources::getResourceFileNames())
        menu.addItem (++index, name);

    return menu;
}

void MagicGUIState::addParametersToMenu (const juce::AudioProcessorParameterGroup& group, juce::PopupMenu& menu, int& index) const
{
    for (const auto& node : group)
    {
        if (const auto* parameter = node->getParameter())
        {
            if (const auto* withID = dynamic_cast<const juce::AudioProcessorParameterWithID*>(parameter))
                menu.addItem (++index, withID->paramID);
        }
        else if (const auto* groupNode = node->getGroup())
        {
            juce::PopupMenu subMenu;
            addParametersToMenu (*groupNode, subMenu, index);
            menu.addSubMenu (groupNode->getName(), subMenu);
        }
    }
}

void MagicGUIState::addPropertiesToMenu (const juce::ValueTree& tree, juce::ComboBox& combo, juce::PopupMenu& menu, const juce::String& path) const
{
    for (const auto& child : tree)
    {
        const auto name = child.getType().toString();
        juce::PopupMenu subMenu;
        addPropertiesToMenu (child, combo, subMenu, path + name + ":");
        menu.addSubMenu (name, subMenu);
    }

    for (int i=0; i < tree.getNumProperties(); ++i)
    {
        const auto name = tree.getPropertyName (i).toString();
        menu.addItem (name, true, false, [&combo, t = path + name]
        {
            combo.setText (t);
        });
    }

    menu.addSeparator();
    menu.addItem (NEEDS_TRANS ("New property"), [&combo, t = path]
    {
        combo.setText (t + ":");
        combo.setEditableText (true);
        combo.showEditor();
    });
}

void MagicGUIState::prepareToPlay (double sampleRate, int samplesPerBlockExpected)
{
    for (auto& plot : advertisedObjects)
        if (auto* source = dynamic_cast<MagicPlotSource*>(plot.second.get()))
            source->prepareToPlay (sampleRate, samplesPerBlockExpected);
}

juce::MidiKeyboardState& MagicGUIState::getKeyboardState()
{
    return keyboardState;
}

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
void MagicGUIState::setResourcesFolder (const juce::String& source)
{
    resourcesFolder = juce::File (source).getParentDirectory();
    auto sibling = resourcesFolder.getSiblingFile ("Resources");
    if (sibling.isDirectory())
        resourcesFolder = sibling;
}

juce::File MagicGUIState::getResourcesFolder() const
{
    return resourcesFolder;
}

#endif // FOLEYS_SHOW_GUI_EDITOR_PALLETTE

} // namespace foleys
