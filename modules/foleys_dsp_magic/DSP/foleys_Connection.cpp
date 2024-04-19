//
// Created by Daniel Walz on 19.04.24.
//

#include "foleys_Connection.h"


namespace foleys::dsp
{

Connection::Connection (DspNode& owner, ConnectionType connectionType, int targetIndexToUse)
  : targetNode (owner), type (connectionType), targetIndex (targetIndexToUse)
{
}

Connection::Connection (const Connection& other) : Connection (other.targetNode, other.type, other.targetIndex)
{
    sourceNode  = other.sourceNode;
    sourceIndex = other.sourceIndex;
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
    return type != Invalid && sourceNode;
}

void Connection::connect (const juce::ValueTree& tree)
{
    jassert (type == Connection::getType (tree));
    jassert (int (tree.getProperty (idTargetIdx, 0)) == targetIndex);

    DBG ("connect: " << tree.toXmlString());

    sourceNode  = targetNode.getProgram().getNodeWithUID (tree.getProperty (idSource, 0));
    sourceIndex = tree.getProperty (idSourceIdx, 0);
}

/* static */
void Connection::connect (std::vector<Connection>& connections, const juce::ValueTree& tree)
{
    auto targetIndex = static_cast<int> (tree.getProperty (idTargetIdx), 0);

    jassert (juce::isPositiveAndBelow (targetIndex, connections.size()));
    connections[targetIndex].connect (tree);
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
Connection::ConnectionType Connection::Connection::getType (const juce::ValueTree& tree)
{
    if (!tree.hasType (idConnection) || !tree.hasProperty (idType))
        return Invalid;

    return getType (tree.getProperty (idType).toString());
}

Connection::ConnectionType Connection::getType (const juce::String& name)
{
    if (name == getTypeName (Audio))
        return Audio;

    if (name == getTypeName (Parameter))
        return Parameter;

    if (name == getTypeName (MIDI))
        return MIDI;

    return Invalid;
}

juce::String Connection::getTypeName (ConnectionType type)
{
    switch (type)
    {
        case Audio: return "audio";
        case Parameter: return "parameter";
        case MIDI: return "midi";
        default: return "invalid";
    }
}


}  // namespace foleys::dsp
