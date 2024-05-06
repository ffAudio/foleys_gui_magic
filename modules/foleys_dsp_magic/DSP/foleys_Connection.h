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

struct Connection
{

    Connection (DspNode& owner, ConnectionType connectionType, const juce::String& name, int targetIndex = 0);

    bool isConnected() const;

    static void connect (ConnectionType type, juce::ValueTree config, int sourceUID, int sourceIdx, int targetIdx);
    static void disconnect (ConnectionType type, juce::ValueTree config, int targetIdx);


    Connection withSource (DspNode* source, int connectionIndex);

    juce::ValueTree   toValueTree();
    static Connection fromValueTree (DspNode& owner, juce::ValueTree tree);

    void        connect (const juce::ValueTree& tree);
    static void connect (std::vector<Connection>& connections, const juce::ValueTree& tree);
    void        disconnect();

    static ConnectionType getType (const juce::ValueTree& tree);
    static ConnectionType getType (const juce::String& name);
    static juce::String   getTypeName (ConnectionType type);

    Output* getOutput() const;


    DspNode&           targetNode;
    ConnectionType     type = ConnectionType::Invalid;
    const juce::String inputName;
    int                targetIndex = 0;

    juce::WeakReference<DspNode> sourceNode;
    int                          sourceIndex = 0;

private:
    static constexpr auto* idConnection = "Connection";
    static constexpr auto* idType       = "type";
    static constexpr auto* idSource     = "source";
    static constexpr auto* idSourceIdx  = "sourceIdx";
    static constexpr auto* idTarget     = "target";
    static constexpr auto* idTargetIdx  = "targetIdx";
};


}  // namespace foleys::dsp
