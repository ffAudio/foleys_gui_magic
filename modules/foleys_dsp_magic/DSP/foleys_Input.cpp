//
// Created by Daniel Walz on 19.04.24.
//

#include "foleys_Input.h"


namespace foleys::dsp
{

Input::Input (DspNode& owner, ConnectionType connectionType, const juce::String& name, int targetIndexToUse)
  : targetNode (owner), type (connectionType), inputName (name), targetIndex (targetIndexToUse)
{
}

Input Input::withSource (DspNode* source, int connectionIndex)
{
    Input out (*this);
    out.sourceNode  = source;
    out.sourceIndex = connectionIndex;
    return out;
}

bool Input::isConnected() const
{
    return type != ConnectionType::Invalid && sourceNode;
}

void Input::connect (ConnectionType type, juce::ValueTree config, int sourceUID, int sourceIdx, int targetIdx, juce::UndoManager* undo)
{
    for (auto child: config)
    {
        if (child.getProperty (idType).toString() == getTypeName (type) && static_cast<int> (child.getProperty (idTargetIdx)) == targetIdx)
        {
            if (undo)
                undo->beginNewTransaction();
            config.setProperty (idSource, sourceUID, undo);
            config.setProperty (idSourceIdx, sourceIdx, undo);
            return;
        }
    }

    juce::ValueTree connection {
        idConnection,
        { { idType, getTypeName (type) }, { idSource, sourceUID }, { idSourceIdx, sourceIdx }, { idTarget, config.getProperty (idTarget) }, { idTargetIdx, targetIdx } }
    };

    if (undo)
        undo->beginNewTransaction();

    config.appendChild (connection, undo);
}

void Input::disconnect (ConnectionType type, juce::ValueTree config, int targetIdx, juce::UndoManager* undo)
{
    for (int i = config.getNumChildren() - 1; i >= 0; --i)
    {
        auto child = config.getChild (i);
        if (child.getProperty (idType).toString() == getTypeName (type) && static_cast<int> (child.getProperty (idTargetIdx)) == targetIdx)
        {
            if (undo)
                undo->beginNewTransaction();

            config.removeChild (i, undo);
        }
    }
}

void Input::connect (const juce::ValueTree& tree)
{
    jassert (type == Input::getType (tree));
    jassert (static_cast<int> (tree.getProperty (idTargetIdx, 0)) == targetIndex);

    sourceNode  = targetNode.getProgram().getNodeWithUID (tree.getProperty (idSource, 0));
    sourceIndex = tree.getProperty (idSourceIdx, 0);
}

/* static */
void Input::connect (std::vector<Input>& connections, const juce::ValueTree& tree)
{
    auto targetIndex = static_cast<int> (tree.getProperty (idTargetIdx, 0));

    if (juce::isPositiveAndBelow (targetIndex, connections.size()))
        connections[static_cast<size_t> (targetIndex)].connect (tree);
    else
        jassertfalse;
}

void Input::disconnect()
{
    sourceNode  = nullptr;
    sourceIndex = 0;
}

Output* Input::getOutput() const
{
    if (!sourceNode)
        return nullptr;

    return sourceNode->getOutput (type, sourceIndex);
}

juce::ValueTree Input::toValueTree()
{
    return juce::ValueTree {
        idConnection,
        { { idType, getTypeName (type) }, { idSource, sourceNode ? sourceNode->getUID() : 0 }, { idSourceIdx, sourceIndex }, { idTarget, targetNode.getUID() }, { idTargetIdx, targetIndex } }
    };
}

/* static */
Input Input::fromValueTree (DspNode& owner, juce::ValueTree tree)
{
    Input out (owner, Input::getType (tree), tree.getProperty (idTargetIdx, 0));
    auto*      source = owner.getProgram().getNodeWithUID (tree.getProperty (idSource, 0));
    return out.withSource (source, tree.getProperty (idSourceIdx, 0));
}

/* static */
ConnectionType Input::Input::getType (const juce::ValueTree& tree)
{
    if (!tree.hasType (idConnection) || !tree.hasProperty (idType))
        return ConnectionType::Invalid;

    return getType (tree.getProperty (idType).toString());
}

ConnectionType Input::getType (const juce::String& name)
{
    if (name == getTypeName (ConnectionType::Audio))
        return ConnectionType::Audio;

    if (name == getTypeName (ConnectionType::Parameter))
        return ConnectionType::Parameter;

    if (name == getTypeName (ConnectionType::MIDI))
        return ConnectionType::MIDI;

    return ConnectionType::Invalid;
}

juce::String Input::getTypeName (ConnectionType type)
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
