//
// Created by Daniel Walz on 19.04.24.
//

#include "foleys_Connection.h"


namespace foleys::dsp
{

Connection::Connection (DspNode& owner, ConnectionType connectionType, const juce::String& name, int targetIndexToUse)
  : targetNode (owner), type (connectionType), inputName (name), targetIndex (targetIndexToUse)
{
}

Connection Connection::withSource (DspNode* source, int connectionIndex)
{
    Connection out (*this);
    out.sourceNode  = source;
    out.sourceIndex = connectionIndex;
    return out;
}

bool Connection::isConnected() const
{
    return type != ConnectionType::Invalid && sourceNode;
}

void Connection::connect (const juce::ValueTree& tree)
{
    jassert (type == Connection::getType (tree));
    jassert (static_cast<int> (tree.getProperty (idTargetIdx, 0)) == targetIndex);

    sourceNode  = targetNode.getProgram().getNodeWithUID (tree.getProperty (idSource, 0));
    sourceIndex = tree.getProperty (idSourceIdx, 0);
}

/* static */
void Connection::connect (std::vector<Connection>& connections, const juce::ValueTree& tree)
{
    auto targetIndex = static_cast<int> (tree.getProperty (idTargetIdx, 0));

    if (juce::isPositiveAndBelow (targetIndex, connections.size()))
        connections[static_cast<size_t> (targetIndex)].connect (tree);
    else
        jassertfalse;
}

Output* Connection::getOutput() const
{
    if (!sourceNode)
        return nullptr;

    return sourceNode->getOutput(type, sourceIndex);
}

juce::ValueTree Connection::toValueTree()
{
    return juce::ValueTree {
        idConnection,
        { { idType, getTypeName (type) }, { idSource, sourceNode ? sourceNode->getUID() : 0 }, { idSourceIdx, sourceIndex }, { idTarget, targetNode.getUID() }, { idTargetIdx, targetIndex } }
    };
}

/* static */
Connection Connection::fromValueTree (DspNode& owner, juce::ValueTree tree)
{
    Connection out (owner, Connection::getType (tree), tree.getProperty (idTargetIdx, 0));
    auto*      source = owner.getProgram().getNodeWithUID (tree.getProperty (idSource, 0));
    return out.withSource (source, tree.getProperty (idSourceIdx, 0));
}

/* static */
ConnectionType Connection::Connection::getType (const juce::ValueTree& tree)
{
    if (!tree.hasType (idConnection) || !tree.hasProperty (idType))
        return ConnectionType::Invalid;

    return getType (tree.getProperty (idType).toString());
}

ConnectionType Connection::getType (const juce::String& name)
{
    if (name == getTypeName (ConnectionType::Audio))
        return ConnectionType::Audio;

    if (name == getTypeName (ConnectionType::Parameter))
        return ConnectionType::Parameter;

    if (name == getTypeName (ConnectionType::MIDI))
        return ConnectionType::MIDI;

    return ConnectionType::Invalid;
}

juce::String Connection::getTypeName (ConnectionType type)
{
    switch (type)
    {
        case ConnectionType::Audio: return "audio";
        case ConnectionType::Parameter: return "parameter";
        case ConnectionType::MIDI: return "midi";
        default: return "invalid";
    }
}


}  // namespace foleys::dsp
