//
// Created by Daniel Walz on 04.02.24.
//

#include "foleys_DspProgram.h"


namespace foleys::dsp
{

DspProgram::DspProgram (MagicDspBuilder& builder, MagicProcessorState& state, const juce::ValueTree& tree, juce::UndoManager* undo)
  : dspBuilder (builder), magicState (state), dspConfig (tree), undoManager (undo)
{
    for (auto node: tree)
        createNode (node);

    updateConnections();
}

bool DspProgram::addNode (const juce::ValueTree& newNode)
{
    auto success = createNode (newNode);
    if (success)
    {
        dspConfig.appendChild (newNode, undoManager);
        updateConnections();
    }

    return success;
}

bool DspProgram::createNode (const juce::ValueTree& newNode)
{
    auto child = dspBuilder.createNode (*this, newNode);
    if (child)
    {
        child->updateConnections();

        auto uid = child->getUID();
        if (uid > 0)
        {
            if (nodeLookup.find (uid) != nodeLookup.cend())
            {
                /*
                 * This uid already exists!
                 */
                jassertfalse;
                return false;
            }
        }
        else
        {
            uid = uidCounter + 1;
            child->setUID (uid);
        }
        uidCounter = std::max (uidCounter, uid);

        nodeLookup[child->getUID()] = child.get();
        nodes.push_back (std::move (child));

        return true;
    }
    return false;
}

bool DspProgram::connectNodes (ConnectionType connectionType, int sourceUID, int sourceIndex, int targetUID, int targetIndex)
{
    auto* targetNode = getNodeWithUID (targetUID);

    if (!targetNode)
        return false;

    if (auto* input = targetNode->getInputChecked(connectionType, targetIndex))
    {
        input->connect(sourceUID, sourceIndex);
    }

    targetNode->updateConnections();

    return true;
}

void DspProgram::disconnect (int nodeUID, ConnectionType connectionType, int connectorIndex, bool isInput)
{
    if (!isInput)
        return; // not implemented, which to disconnect?

    auto* targetNode = getNodeWithUID (nodeUID);

    if (!targetNode)
        return;

    if (auto* input = targetNode->getInputChecked(connectionType, connectorIndex))
    {
        input->disconnect();
    }
}

void DspProgram::prepareToPlay (double sampleRate, int expectedNumSamples)
{
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (expectedNumSamples), 2 };

    for (const auto& node: nodes)
        node->prepare (spec);
}

void DspProgram::processBlock (juce::AudioProcessor& processor, juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midi)
{
    for (size_t i = 0; i < audioInputs.size(); ++i)
    {
        auto busBuffer = processor.getBusBuffer (buffer, true, static_cast<int> (i));
        audioInputs[i]->setAudioBuffer (busBuffer.getArrayOfWritePointers(), busBuffer.getNumChannels(), busBuffer.getNumSamples());
    }

    for (size_t i = 0; i < audioOutputs.size(); ++i)
    {
        auto busBuffer = processor.getBusBuffer (buffer, false, static_cast<int> (i));
        audioOutputs[i]->setAudioBuffer (busBuffer.getArrayOfWritePointers(), busBuffer.getNumChannels(), busBuffer.getNumSamples());
    }

    for (auto* node: order)
        node->process (buffer.getNumSamples());
}

void DspProgram::releaseResources()
{
    for (const auto& node: nodes)
        node->release();
}

DspNode* DspProgram::getNodeWithUID (int uid)
{
    const auto& it = std::find_if (nodes.begin(), nodes.end(), [uid] (const auto& node) { return node->getUID() == uid; });
    if (it != nodes.end())
        return it->get();

    return nullptr;
}

void DspProgram::updateConnections()
{
    audioInputs.clear();
    audioOutputs.clear();
    order.clear();

    midiInput  = nullptr;
    midiOutput = nullptr;

    for (const auto& node: nodes)
    {
        if (auto* input = dynamic_cast<AudioInput*> (node.get()))
            audioInputs.emplace_back (input);
        else if (auto* output = dynamic_cast<AudioOutput*> (node.get()))
            audioOutputs.emplace_back (output);
        else if (auto* pInput = dynamic_cast<ParameterInput*> (node.get()))
            parameterInputs.emplace_back (pInput);
        else if (auto* mInput = dynamic_cast<MidiInput*> (node.get()))
            midiInput = mInput;
        else if (auto* mOutput = dynamic_cast<MidiOutput*> (node.get()))
            midiOutput = mOutput;
    }

    if (auto* processor = magicState.getProcessor())
    {
        const auto& tree = processor->getParameterTree();

        for (const auto& pn: parameterInputs)
        {
            pn->setParameterGroup (tree);
        }
    }


    for (const auto& node: nodes)
        node->updateConnections();

    // determine order:

    while (order.size() < nodes.size())
    {
        for (const auto& node: nodes)
        {
            if (std::find (order.begin(), order.end(), node.get()) != order.end())
                continue;

            auto dependsOn = node->getNodesToDependOn();

            dependsOn.erase (std::remove_if (dependsOn.begin(), dependsOn.end(),
                                             [&] (const auto& n) { return std::find (order.begin(), order.end(), n) != order.end(); }),
                             dependsOn.end());

            if (dependsOn.empty())
                order.push_back (node.get());
        }
    }
}


}  // namespace foleys::dsp
