//
// Created by Daniel Walz on 19.04.24.
//

#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

namespace foleys::dsp
{

class DspNode;
struct Output;

enum class ConnectionType
{
    Invalid = 0,
    Audio,
    MIDI,
    Parameter
};

struct Input
{

    Input (DspNode& owner, ConnectionType connectionType, const juce::String& name, int targetIndex = 0);

    bool isConnected() const;

    juce::ValueTree toValueTree();

    juce::ValueTree getConfigForInput();
    void            save();
    void            restore();

    void setRange (float minValue, float maxValue);
    void setDefaultValue (float value);

    void connect (int sourceUID, int sourceIndex);
    void disconnect();

    static ConnectionType getType (const juce::ValueTree& tree);
    static ConnectionType getType (const juce::String& name);
    static juce::String   getTypeName (ConnectionType type);

    [[nodiscard]] Output* getOutput() const;

    DspNode&           targetNode;
    ConnectionType     type = ConnectionType::Invalid;
    const juce::String inputName;
    int                targetIndex = 0;

    float minValue     = 0.0f;
    float maxValue     = 1.0f;
    float defaultValue = 0.0f;

    juce::WeakReference<DspNode> sourceNode;
    int                          sourceIndex = 0;

private:
    static constexpr auto* idConnection = "Connection";
    static constexpr auto* idType       = "type";
    static constexpr auto* idSource     = "source";
    static constexpr auto* idSourceIdx  = "sourceIdx";
    static constexpr auto* idTarget     = "target";
    static constexpr auto* idTargetIdx  = "targetIdx";
    static constexpr auto* idMinValue   = "minValue";
    static constexpr auto* idMaxValue   = "maxValue";
    static constexpr auto* idDefault    = "default";
};


}  // namespace foleys::dsp
