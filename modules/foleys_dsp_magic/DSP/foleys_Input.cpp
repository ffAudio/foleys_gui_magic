//
// Created by Daniel Walz on 19.04.24.
//

#include "foleys_Input.h"


namespace foleys::dsp
{

Input::Input (DspNode& owner, ConnectionType connectionType, const juce::String& name, int targetIndexToUse)
  : targetNode (owner), type (connectionType), inputName (name), targetIndex (targetIndexToUse)
{
    restore();
}

void Input::setRange (float min, float max)
{
    minValue = min;
    maxValue = max;

    save();
}

void Input::setDefaultValue (float value)
{
    defaultValue = value;

    save();
}

void Input::save()
{
    auto node = getConfigForInput();

    if (node.isValid())
        node.copyPropertiesFrom (toValueTree(), nullptr);
    else
        targetNode.getConfig().appendChild (toValueTree(), nullptr);
}

void Input::restore()
{
    auto node = getConfigForInput();

    if (!node.isValid())
        return;

    minValue     = node.getProperty (idMinValue, 0.0f);
    maxValue     = node.getProperty (idMaxValue, 1.0f);
    defaultValue = node.getProperty (idDefault, 0.0f);
    sourceNode   = targetNode.getProgram().getNodeWithUID (node.getProperty (idSource, 0));
    sourceIndex  = node.getProperty (idSourceIdx, -1);
}

bool Input::isConnected() const
{
    return type != ConnectionType::Invalid && sourceNode;
}

void Input::connect (int sourceUID, int sourceIdx)
{
    sourceNode  = targetNode.getProgram().getNodeWithUID (sourceUID);
    sourceIndex = sourceIdx;

    save();
}

void Input::disconnect()
{
    sourceNode  = nullptr;
    sourceIndex = 0;

    save();
}

Output* Input::getOutput() const
{
    if (!sourceNode)
        return nullptr;

    return sourceNode->getOutput (type, sourceIndex);
}

bool Input::isStatic() const
{
    if (auto* output = getOutput())
        return output->isStatic();

    return true;
}

float Input::getStaticValue() const
{
    if (auto* output = getOutput())
    {
        auto staticValue = output->getStaticValue();
        return (minValue + output->normalize (staticValue) * (maxValue - minValue));
    }

    return defaultValue;
}

void Input::mapOutput()
{
    if (auto* output = getOutput())
    {
        auto srcMin = output->getRangeMin();
        auto srcMax = output->getRangeMax();

        if (juce::approximatelyEqual (srcMin, minValue) && juce::approximatelyEqual (srcMax, maxValue))
            return;

        auto  block   = output->getAudio();
        auto* samples = block.getChannelPointer (0);

        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            samples[i] = minValue + ((samples[i] - srcMin) / (srcMax - srcMin)) * (maxValue - minValue);
        }
    }
}

juce::ValueTree Input::getConfigForInput()
{
    for (const auto& child: targetNode.getConfig())
        if (child.hasType (idConnection) && child.getProperty (idType).toString() == getTypeName (type) && static_cast<int> (child.getProperty (idTargetIdx)) == targetIndex)
            return child;

    return {};
}

juce::ValueTree Input::toValueTree()
{
    return juce::ValueTree { idConnection,
                             { { idType, getTypeName (type) },
                               { idSource, sourceNode ? sourceNode->getUID() : 0 },
                               { idSourceIdx, sourceIndex },
                               { idTarget, targetNode.getUID() },
                               { idTargetIdx, targetIndex },
                               { idMinValue, minValue },
                               { idMaxValue, maxValue },
                               { idDefault, defaultValue } } };
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
        case ConnectionType::Invalid: [[fallthrough]];
        default: return "invalid";
    }
}


}  // namespace foleys::dsp
