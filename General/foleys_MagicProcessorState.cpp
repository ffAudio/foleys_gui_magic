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

MagicProcessorState::MagicProcessorState (juce::AudioProcessor& processorToUse,
                                          juce::AudioProcessorValueTreeState& stateToUse)
  : processor (processorToUse),
    state (stateToUse)
{
}

MagicProcessorState::~MagicProcessorState()
{
    visualiserThread.stopThread (1000);
}

void MagicProcessorState::addBackgroundProcessing (MagicPlotSource* source)
{
    if (auto* job = source->getBackgroundJob())
    {
        visualiserThread.addTimeSliceClient (job);
        visualiserThread.startThread (3);
    }
}

void MagicProcessorState::addTrigger (const juce::Identifier& triggerID, std::function<void()> function)
{
    triggers [triggerID] = function;
}

std::function<void()> MagicProcessorState::getTrigger (const juce::Identifier& triggerID)
{
    auto it = triggers.find (triggerID);
    if (it == triggers.end())
        return nullptr;

    return it->second;
}

juce::ValueTree MagicProcessorState::getPropertyRoot() const
{
    return state.state.getOrCreateChildWithName ("properties", nullptr);
}

juce::Value MagicProcessorState::getPropertyAsValue (const juce::String& pathToProperty)
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

juce::StringArray MagicProcessorState::getParameterNames() const
{
    juce::StringArray names;
    for (const auto* p : processor.getParameters())
        if (const auto* withID = dynamic_cast<const juce::AudioProcessorParameterWithID*>(p))
            names.add (withID->paramID);

    return names;
}

void MagicProcessorState::populatePropertiesMenu (juce::ComboBox& comboBox) const
{
    addPropertiesToMenu (getPropertyRoot(), comboBox, *comboBox.getRootMenu(), {});
}

juce::PopupMenu MagicProcessorState::createParameterMenu() const
{
    juce::PopupMenu menu;
    int index = 0;
    addParametersToMenu (processor.getParameterTree(), menu, index);
    return menu;
}

juce::PopupMenu MagicProcessorState::createPropertiesMenu (juce::ComboBox& combo) const
{
    juce::PopupMenu menu;
    addPropertiesToMenu (getPropertyRoot(), combo, menu, {});
    return menu;
}

juce::PopupMenu MagicProcessorState::createTriggerMenu() const
{
    juce::PopupMenu menu;
    int index = 0;
    for (const auto& trigger : triggers)
        menu.addItem (++index, trigger.first.toString());

    return menu;
}

juce::PopupMenu MagicProcessorState::createAssetFilesMenu() const
{
    juce::PopupMenu menu;
    int index = 0;
    for (const auto& name : Resources::getResourceFileNames())
        menu.addItem (++index, name);

    return menu;
}

void MagicProcessorState::addParametersToMenu (const juce::AudioProcessorParameterGroup& group, juce::PopupMenu& menu, int& index) const
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

void MagicProcessorState::addPropertiesToMenu (const juce::ValueTree& tree, juce::ComboBox& combo, juce::PopupMenu& menu, const juce::String& path) const
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
        menu.addItem (name, [&combo, t = path + name]
        {
            combo.setText (t);
        });
    }

    menu.addSeparator();
    menu.addItem (NEEDS_TRANS ("New property"), [&combo, t = path]
    {
        combo.setText (t + ":");
        combo.showEditor();
    });
}

void MagicProcessorState::prepareToPlay (double sampleRate, int samplesPerBlockExpected)
{
    for (auto& plot : advertisedObjects)
        if (auto* source = dynamic_cast<MagicPlotSource*>(plot.second.get()))
            source->prepareToPlay (sampleRate, samplesPerBlockExpected);
}

void MagicProcessorState::processMidiBuffer (juce::MidiBuffer& buffer, int numSamples, bool injectIndirectEvents)
{
    keyboardState.processNextMidiBuffer (buffer, 0, numSamples, injectIndirectEvents);
}

juce::AudioProcessor& MagicProcessorState::getProcessor()
{
    return processor;
}

void MagicProcessorState::setLastEditorSize (int  width, int  height)
{
    if (state.state.isValid() == false)
        return;

    auto sizeNode = state.state.getOrCreateChildWithName (IDs::lastSize, nullptr);
    sizeNode.setProperty (IDs::width,  width,  nullptr);
    sizeNode.setProperty (IDs::height, height, nullptr);
}

bool MagicProcessorState::getLastEditorSize (int& width, int& height)
{
    if (state.state.isValid() == false)
        return false;

    auto sizeNode = state.state.getOrCreateChildWithName (IDs::lastSize, nullptr);
    if (sizeNode.hasProperty (IDs::width) == false || sizeNode.hasProperty (IDs::height) == false)
        return false;

    width  = sizeNode.getProperty (IDs::width);
    height = sizeNode.getProperty (IDs::height);
    return true;
}

void MagicProcessorState::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, false);
    state.state.writeToStream (stream);
}

void MagicProcessorState::setStateInformation (const void* data, int sizeInBytes, juce::AudioProcessorEditor* editor)
{
    auto tree = juce::ValueTree::readFromData (data, size_t (sizeInBytes));
    if (tree.isValid() == false)
        return;

    state.replaceState (tree);

    if (editor)
    {
        int width, height;

        if (getLastEditorSize (width, height))
            editor->setSize (width, height);
    }
}

void MagicProcessorState::updatePlayheadInformation (juce::AudioPlayHead* playhead)
{
    if (playhead == nullptr)
        return;

    juce::AudioPlayHead::CurrentPositionInfo info;
    playhead->getCurrentPosition (info);

    bpm.store (info.bpm);
    timeInSeconds.store (info.timeInSeconds);
    timeSigNumerator.store (info.timeSigNumerator);
    timeSigDenominator.store (info.timeSigDenominator);
    isPlaying.store (info.isPlaying);
    isRecording.store (info.isRecording);
}

void MagicProcessorState::setPlayheadUpdateFrequency (int frequency)
{
    if (frequency > 0)
        startTimerHz (frequency);
    else
        stopTimer();
}

void MagicProcessorState::timerCallback()
{
    getPropertyAsValue ("playhead:bpm").setValue (bpm.load());
    getPropertyAsValue ("playhead:timeInSeconds").setValue (timeInSeconds.load());
    getPropertyAsValue ("playhead:timeSigNumerator").setValue (timeSigNumerator.load());
    getPropertyAsValue ("playhead:timeSigDenominator").setValue (timeSigDenominator.load());
    getPropertyAsValue ("playhead:isPlaying").setValue (isPlaying.load());
    getPropertyAsValue ("playhead:isRecording").setValue (isRecording.load());
}

juce::AudioProcessorValueTreeState& MagicProcessorState::getValueTreeState()
{
    return state;
}

juce::MidiKeyboardState& MagicProcessorState::getKeyboardState()
{
    return keyboardState;
}

} // namespace foleys
