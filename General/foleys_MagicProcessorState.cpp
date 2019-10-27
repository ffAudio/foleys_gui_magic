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

void MagicProcessorState::addLevelSource (const juce::Identifier& sourceID, std::unique_ptr<MagicLevelSource> source)
{
    if (levelSources.find (sourceID) != levelSources.cend())
    {
        // You tried to add two MagicLevelSources with the same sourceID
        jassertfalse;
        return;
    }

    levelSources [sourceID] = std::move (source);
}

MagicLevelSource* MagicProcessorState::getLevelSource (const juce::Identifier& sourceID)
{
    auto it = levelSources.find (sourceID);
    if (it != levelSources.end())
        return nullptr;

    return it->second.get();
}

MagicPlotSource* MagicProcessorState::addPlotSource (const juce::Identifier& sourceID, std::unique_ptr<MagicPlotSource> source)
{
    const auto& present = plotSources.find (sourceID);
    if (present != plotSources.cend())
    {
        // You tried to add two MagicPlotSources with the same sourceID
        jassertfalse;
        return present->second.get();
    }

    if (auto* job = source->getBackgroundJob())
    {
        visualiserThread.addTimeSliceClient (job);
        visualiserThread.startThread (3);
    }

    auto* pointer = source.get();
    plotSources [sourceID] = std::move (source);
    return pointer;
}

MagicPlotSource* MagicProcessorState::getPlotSource (const juce::Identifier& sourceID)
{
    auto it = plotSources.find (sourceID);
    if (it == plotSources.end())
        return nullptr;

    return it->second.get();
}

juce::StringArray MagicProcessorState::getParameterNames() const
{
    juce::StringArray names;
    for (const auto* p : processor.getParameters())
        if (const auto* withID = dynamic_cast<const juce::AudioProcessorParameterWithID*>(p))
            names.add (withID->paramID);

    return names;
}

juce::StringArray MagicProcessorState::getPlotSourcesNames() const
{
    juce::StringArray names;
    for (const auto& p : plotSources)
        names.add (p.first.toString());

    return names;
}

void MagicProcessorState::prepareToPlay (double sampleRate, int samplesPerBlockExpected)
{
    for (auto& plot : plotSources)
        plot.second->prepareToPlay (sampleRate, samplesPerBlockExpected);
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

    state.state = tree;

    if (editor)
    {
        int width, height;

        if (getLastEditorSize (width, height))
            editor->setSize (width, height);
    }
}

juce::AudioProcessorValueTreeState& MagicProcessorState::getValueTreeState()
{
    return state;
}

} // namespace foleys
