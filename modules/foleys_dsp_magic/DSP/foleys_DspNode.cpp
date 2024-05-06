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

void DspNode::addAudioInput (const juce::String& name)
{
    audioInputs.emplace_back (*this, ConnectionType::Audio, name, static_cast<int> (audioInputs.size()));
}

void DspNode::addParameterInput (const juce::String& name)
{
    parameterInputs.emplace_back (*this, ConnectionType::Parameter, name, static_cast<int> (parameterInputs.size()));
}

void DspNode::addAudioOutput (const juce::String& name)
{
    audioOutputs.emplace_back (*this, name);
}

void DspNode::addParameterOutput (const juce::String& name)
{
    parameterOutputs.emplace_back (*this, name);
}

Output* DspNode::getOutput (ConnectionType type, int index)
{
    if (type == ConnectionType::Audio && juce::isPositiveAndBelow (index, audioOutputs.size()))
        return &audioOutputs[static_cast<size_t> (index)];

    if (type == ConnectionType::Parameter && juce::isPositiveAndBelow (index, parameterOutputs.size()))
        return &parameterOutputs[static_cast<size_t> (index)];

    return nullptr;
}

void DspNode::updateConnections()
{
    for (auto& audioInput: getAudioInputs())
        audioInput.disconnect();
    for (auto& parameterInput: getParameterInputs())
        parameterInput.disconnect();

    midiInput.disconnect();

    for (const auto& connection: config)
    {
        switch (Input::getType (connection))
        {
            case ConnectionType::MIDI: midiInput.connect (connection); break;
            case ConnectionType::Audio: Input::connect (audioInputs, connection); break;
            case ConnectionType::Parameter: Input::connect (parameterInputs, connection); break;

            default: break;
        }
    }
}

Output* DspNode::getConnectedOutput (ConnectionType type, int inputIndex)
{
    if (auto* connection = getInput (type, inputIndex))
    {
        jassert (inputIndex == connection->targetIndex);

        if (connection->isConnected())
            return connection->sourceNode->getOutput (type, connection->sourceIndex);
    }

    return nullptr;
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

Input* DspNode::getInput (ConnectionType type, int index)
{
    switch (type)
    {
        case ConnectionType::MIDI: return &midiInput;
        case ConnectionType::Audio: return &audioInputs[static_cast<size_t> (index)];
        case ConnectionType::Parameter: return &parameterInputs[static_cast<size_t> (index)];
        case ConnectionType::Invalid: return nullptr;
        default: jassertfalse; return nullptr;
    }
}

int DspNode::getNumAudioInputs() const
{
    return static_cast<int> (audioInputs.size());
}

int DspNode::getNumParameterInputs() const
{
    return static_cast<int> (parameterInputs.size());
}

int DspNode::getNumAudioOutputs() const
{
    return static_cast<int> (audioOutputs.size());
}

int DspNode::getNumParameterOutputs() const
{
    return static_cast<int> (parameterOutputs.size());
}

juce::String DspNode::getAudioInputName (int index) const
{
    if (juce::isPositiveAndBelow (index, audioInputs.size()))
        return audioInputs[static_cast<size_t> (index)].inputName;

    jassertfalse;
    return TRANS ("Audio ") + juce::String (index);
}

juce::String DspNode::getParameterInputName (int index) const
{
    if (juce::isPositiveAndBelow (index, parameterInputs.size()))
        return parameterInputs[static_cast<size_t> (index)].inputName;

    jassertfalse;
    return TRANS ("Parameter ") + juce::String (index);
}

juce::String DspNode::getAudioOutputName (int index) const
{
    if (juce::isPositiveAndBelow (index, audioOutputs.size()))
        return audioOutputs[static_cast<size_t> (index)].name;

    jassertfalse;
    return TRANS ("Audio ") + juce::String (index);
}

juce::String DspNode::getParameterOutputName (int index) const
{
    if (juce::isPositiveAndBelow (index, parameterOutputs.size()))
        return parameterOutputs[static_cast<size_t> (index)].name;

    jassertfalse;
    return TRANS ("Parameter ") + juce::String (index);
}

juce::String DspNode::getMidiOutputName() const
{
    return TRANS ("Midi Out");
}

juce::String DspNode::getMidiInputName() const
{
    return TRANS ("Midi In");
}

void DspNode::clearInputs (ConnectionType type)
{
    switch (type)
    {
        case ConnectionType::Audio: audioInputs.clear(); break;
        case ConnectionType::Parameter: parameterInputs.clear(); break;
        default: break;
    }
}

void DspNode::clearOutputs (ConnectionType type)
{
    switch (type)
    {
        case ConnectionType::Audio: audioOutputs.clear(); break;
        case ConnectionType::Parameter: parameterOutputs.clear(); break;
        default: break;
    }
}

std::vector<DspNode*> DspNode::getNodesToDependOn()
{
    std::vector<DspNode*> sources;
    for (const auto& audio: audioInputs)
        if (audio.isConnected())
            sources.push_back (audio.sourceNode);

    for (const auto& parameter: parameterInputs)
        if (parameter.isConnected())
            sources.push_back (parameter.sourceNode);

    if (midiInput.isConnected())
        sources.push_back (midiInput.sourceNode);

    return sources;
}


}  // namespace foleys::dsp
