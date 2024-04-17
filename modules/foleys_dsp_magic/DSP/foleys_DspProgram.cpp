//
// Created by Daniel Walz on 04.02.24.
//

#include "foleys_DspProgram.h"


namespace foleys::dsp
{

DspProgram::DspProgram (MagicDspBuilder& builder) : dspBuilder (builder) { }

DspProgram::DspProgram (MagicDspBuilder& builder, const juce::ValueTree& tree) : dspBuilder (builder), dspConfig (tree)
{
    for (auto node: tree)
    {
        createNode (node);
    }

    for (const auto& node: nodes)
    {
        if (auto* input = dynamic_cast<AudioInput*> (node.get()))
            audioInputs.emplace_back (input);
        else if (auto* output = dynamic_cast<AudioOutput*> (node.get()))
            audioOutputs.emplace_back (output);
        else if (auto* minput = dynamic_cast<MidiInput*> (node.get()))
            midiInput = minput;
        else if (auto* moutput = dynamic_cast<MidiOutput*> (node.get()))
            midiOutput = moutput;
    }
}

bool DspProgram::addNode (const juce::ValueTree& newNode)
{
    auto success = createNode (newNode);
    if (success)
        dspConfig.appendChild (newNode, nullptr);

    return success;
}

bool DspProgram::createNode (const juce::ValueTree& newNode)
{
    auto child = dspBuilder.createNode (*this, newNode);
    if (child)
    {
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

bool DspProgram::connectNodes (DspNode::ConnectionType connectionType, int sourceUID, int sourceIndex, int targetUID, int targetIndex)
{

}


void DspProgram::prepareToPlay (double sampleRate, int expectedNumSamples)
{
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (expectedNumSamples), 2 };

    for (const auto& node: nodes)
        node->prepare (spec);
}

void DspProgram::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) { }

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


}  // namespace foleys::dsp
