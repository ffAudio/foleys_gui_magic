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

#include "foleys_MagicProcessorState.h"

namespace foleys
{

MagicProcessorState::MagicProcessorState (juce::AudioProcessor& processorToUse)
  : processor (processorToUse)
{
}

MagicProcessorState::~MagicProcessorState()
{
    stopTimer();
}

juce::StringArray MagicProcessorState::getParameterNames() const
{
    return parameters.getParameterNames();
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
    return parameters.getParameter (paramID);
}

void MagicProcessorState::updateParameterMap()
{
    parameters.updateParameterMap();
}

std::unique_ptr<juce::SliderParameterAttachment> MagicProcessorState::createAttachment (const juce::String& paramID, juce::Slider& slider)
{
    if (auto* parameter = getParameter (paramID))
        return std::make_unique<juce::SliderParameterAttachment>(*parameter, slider);

    // You have connected a control to a parameter that doesn't exist. Please fix your GUI.
    // You may safely click continue in your debugger
    jassertfalse;
    return {};
}

std::unique_ptr<juce::ComboBoxParameterAttachment> MagicProcessorState::createAttachment (const juce::String& paramID, juce::ComboBox& combobox)
{
    if (auto* parameter = getParameter (paramID))
        return std::make_unique<juce::ComboBoxParameterAttachment>(*parameter, combobox);

    // You have connected a control to a parameter that doesn't exist. Please fix your GUI.
    // You may safely click continue in your debugger
    jassertfalse;
    return {};
}

std::unique_ptr<juce::ButtonParameterAttachment> MagicProcessorState::createAttachment (const juce::String& paramID, juce::Button& button)
{
    if (auto* parameter = getParameter (paramID))
        return std::make_unique<juce::ButtonParameterAttachment>(*parameter, button);

    // You have connected a control to a parameter that doesn't exist. Please fix your GUI.
    // You may safely click continue in your debugger
    jassertfalse;
    return {};
}

juce::AudioProcessor* MagicProcessorState::getProcessor()
{
    return &processor;
}

void MagicProcessorState::setLastEditorSize (int  width, int  height)
{
    auto sizeNode = getValueTree().getOrCreateChildWithName (IDs::lastSize, nullptr);
    sizeNode.setProperty (IDs::width,  width,  nullptr);
    sizeNode.setProperty (IDs::height, height, nullptr);
}

bool MagicProcessorState::getLastEditorSize (int& width, int& height)
{
    auto sizeNode = getValueTree().getOrCreateChildWithName (IDs::lastSize, nullptr);
    if (sizeNode.hasProperty (IDs::width) == false || sizeNode.hasProperty (IDs::height) == false)
        return false;

    width  = sizeNode.getProperty (IDs::width);
    height = sizeNode.getProperty (IDs::height);
    return true;
}

void MagicProcessorState::getStateInformation (juce::MemoryBlock& destData)
{
    auto newState = getValueTree();
    parameters.saveParameterValues (newState);

    juce::MemoryOutputStream stream (destData, false);
    newState.writeToStream (stream);
}

void MagicProcessorState::setStateInformation (const void* data, int sizeInBytes, juce::AudioProcessorEditor* editor)
{
    auto tree = juce::ValueTree::readFromData (data, size_t (sizeInBytes));
    if (tree.isValid() == false)
        return;

    auto newState = getValueTree();
    if (newState.getType() != tree.getType())
        return;

    newState.copyPropertiesAndChildrenFrom (tree, nullptr);

    parameters.loadParameterValues (newState);

    if (editor)
    {
        int width, height;

        if (getLastEditorSize (width, height))
        {
            juce::Component::SafePointer safeEditor (editor);
            juce::MessageManager::callAsync([safeEditor, width, height]
                                            {
                if (safeEditor)
                    safeEditor->setSize (width, height);
                                            });
        }
    }
}

void MagicProcessorState::updatePlayheadInformation (juce::AudioPlayHead* playhead)
{
    if (playhead == nullptr)
        return;

    if (const auto position = playhead->getPosition())
    {
        if (auto seconds = position->getTimeInSeconds())
            timeInSeconds.store (*seconds);

        if (auto currentBpm = position->getBpm())
            bpm.store (*currentBpm);

        if (auto signature = position->getTimeSignature())
        {
            timeSigNumerator.store (signature->numerator);
            timeSigDenominator.store ((*signature).denominator);
        }

        isPlaying.store (position->getIsPlaying());
        isRecording.store (position->getIsRecording());
    }
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

void MagicProcessorState::timerCallback()
{
    getPropertyAsValue ("playhead:bpm").setValue (bpm.load());
    getPropertyAsValue ("playhead:timeInSeconds").setValue (timeInSeconds.load());
    getPropertyAsValue ("playhead:timeSigNumerator").setValue (timeSigNumerator.load());
    getPropertyAsValue ("playhead:timeSigDenominator").setValue (timeSigDenominator.load());
    getPropertyAsValue ("playhead:isPlaying").setValue (isPlaying.load());
    getPropertyAsValue ("playhead:isRecording").setValue (isRecording.load());
}

} // namespace foleys
