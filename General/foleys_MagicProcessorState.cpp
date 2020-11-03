/*
 ==============================================================================
    Copyright (c) 2019-2020 Foleys Finest Audio Ltd. - Daniel Walz
    All rights reserved.

    License for non-commercial projects:

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

    License for commercial products:

    To sell commercial products containing this module, you are required to buy a
    License from https://foleysfinest.com/developer/pluginguimagic/

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
  : MagicGUIState(),
    processor (processorToUse),
    state (stateToUse)
{
    midiMapper.setAudioProcessorValueTreeState (&state);
}

juce::ValueTree MagicProcessorState::getPropertyRoot() const
{
    return state.state.getOrCreateChildWithName ("properties", nullptr);
}

juce::StringArray MagicProcessorState::getParameterNames() const
{
    juce::StringArray names;
    for (const auto* p : processor.getParameters())
        if (const auto* withID = dynamic_cast<const juce::AudioProcessorParameterWithID*>(p))
            names.add (withID->paramID);

    return names;
}

juce::PopupMenu MagicProcessorState::createParameterMenu() const
{
    juce::PopupMenu menu;
    int index = 0;
    addParametersToMenu (processor.getParameterTree(), menu, index);
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

juce::RangedAudioParameter* MagicProcessorState::getParameter (const juce::String& paramID)
{
    return state.getParameter (paramID);
}

std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> MagicProcessorState::createAttachment (const juce::String& paramID, juce::Slider& slider)
{
    return std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, paramID, slider);
}

std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> MagicProcessorState::createAttachment (const juce::String& paramID, juce::ComboBox& combobox)
{
    return std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, paramID, combobox);
}

std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> MagicProcessorState::createAttachment (const juce::String& paramID, juce::Button& button)
{
    return std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, paramID, button);
}

juce::AudioProcessor* MagicProcessorState::getProcessor()
{
    return &processor;
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
    juce::MemoryOutputStream stream (destData, false);
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

void MagicProcessorState::processMidiBuffer (juce::MidiBuffer& buffer, int numSamples, bool injectIndirectEvents)
{
    getKeyboardState().processNextMidiBuffer (buffer, 0, numSamples, injectIndirectEvents);

    midiMapper.processMidiBuffer (buffer);
}

void MagicProcessorState::mapMidiController (int cc, const juce::String& parameterID)
{
    midiMapper.mapMidiController (cc, parameterID);
}

int MagicProcessorState::getLastController() const
{
    return midiMapper.getLastController();
}

juce::ValueTree MagicProcessorState::createDefaultGUITree() const
{
    juce::ValueTree rootNode {IDs::view, {{ IDs::id, IDs::root }}};

    auto current = rootNode;
    auto plotNames = getObjectIDsByType<MagicPlotSource>();

    if (plotNames.isEmpty() == false)
    {
        juce::StringArray colours { "orange", "blue", "red", "silver", "green", "cyan", "brown", "white" };
        int nextColour = 0;

        juce::ValueTree child { IDs::view, {
            { IDs::id, "plot-view" },
            { IDs::styleClass, "plot-view" }}};

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

    createDefaultFromParameters (current, processor.getParameterTree());

    return rootNode;
}

void MagicProcessorState::createDefaultFromParameters (juce::ValueTree& node, const juce::AudioProcessorParameterGroup& tree) const
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

} // namespace foleys
