//
// Created by Daniel Walz on 04.02.24.
//

#include "foleys_DspNode.h"

namespace foleys::dsp
{

namespace NodeIDs
{
DECLARE_ID (name)
DECLARE_ID (uid)
}  // namespace NodeIDs

DspNode::DspNode (DspProgram& owningProgram, const juce::ValueTree& node) : program (owningProgram), config (node)
{
    nodeType = node.getType().toString();
}

DspNode::~DspNode()
{
    masterReference.clear();
}

void DspNode::updateConnections()
{
    for (int i = static_cast<int> (audioInputs.size()); i < getNumAudioInputs(); ++i)
        audioInputs.emplace_back (*this, Connection::Audio, i);

    for (int i = static_cast<int> (parameterInputs.size()); i < getNumParameterInputs(); ++i)
        parameterInputs.emplace_back (*this, Connection::Parameter, i);

    for (const auto& connection: config)
    {
        switch (Connection::getType (connection))
        {
            case Connection::MIDI: midiInput.connect (connection); break;
            case Connection::Audio: Connection::connect (audioInputs, connection); break;
            case Connection::Parameter: Connection::connect (parameterInputs, connection); break;

            default: break;
        }
    }
}


/* static */
int DspNode::getUID (const juce::ValueTree& tree)
{
    return tree.getProperty (NodeIDs::uid, 0);
}

juce::String DspNode::getName() const
{
    return config.getProperty (NodeIDs::name, "unnamed").toString();
}

int DspNode::getUID() const
{
    return config.getProperty (NodeIDs::uid, 0);
}

void DspNode::setName (const juce::String& newName)
{
    config.setProperty (NodeIDs::name, newName, nullptr);
}

void DspNode::setUID (int newUID)
{
    config.setProperty (NodeIDs::uid, newUID, nullptr);
}

const Connection* DspNode::getConnection (Connection::ConnectionType type, int index) const
{
    return getConnection (type, index);
}

Connection* DspNode::getConnection (Connection::ConnectionType type, int index)
{
    switch (type)
    {
        case Connection::MIDI: return &midiInput;
        case Connection::Audio: return &audioInputs[static_cast<size_t> (index)];
        case Connection::Parameter: return &parameterInputs[static_cast<size_t> (index)];
        default: jassertfalse; return nullptr;
    }
}


}  // namespace foleys::dsp
