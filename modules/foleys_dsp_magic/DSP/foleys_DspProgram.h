//
// Created by Daniel Walz on 04.02.24.
//

#pragma once

#include "foleys_dsp_magic/Nodes/foleys_BuiltinNodes.h"

#include <foleys_gui_magic/foleys_gui_magic.h>
#include <juce_dsp/juce_dsp.h>

namespace foleys::dsp
{

class DspProgram
{
public:
    DspProgram (MagicDspBuilder& builder, MagicProcessorState& magicState, const juce::ValueTree& tree, juce::UndoManager* undoManager);

    bool addNode (const juce::ValueTree& newNode);
    bool createNode (const juce::ValueTree& newNode);

    bool connectNodes (ConnectionType connectionType, int sourceUID, int sourceIndex, int targetUID, int targetIndex);
    void disconnect (int nodeUID, ConnectionType connectionType, int connectorIndex, bool input);

    void prepareToPlay (double sampleRate, int expectedNumSamples);
    void processBlock (juce::AudioProcessor& processor, juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi);
    void releaseResources();

    bool acceptsMidi() const { return midiInput != nullptr; }
    bool producesMidi() const { return midiOutput != nullptr; }
    bool isMidiEffect() const { return acceptsMidi() && producesMidi(); }

    DspNode* getNodeWithUID (int uid);

    std::vector<std::unique_ptr<DspNode>>::const_iterator begin() { return nodes.cbegin(); }
    std::vector<std::unique_ptr<DspNode>>::const_iterator end() { return nodes.cend(); }

    [[nodiscard]] juce::ValueTree getConfig() const { return dspConfig; }
    MagicProcessorState&          getMagicProcessorState() { return magicState; }

private:
    void updateConnections();

    MagicDspBuilder&     dspBuilder;
    MagicProcessorState& magicState;

    juce::UndoManager* undoManager = nullptr;
    juce::ValueTree    dspConfig { "Program" };

    std::vector<std::unique_ptr<DspNode>> nodes;
    std::map<int, DspNode*>               nodeLookup;
    int                                   uidCounter = 0;

    MidiInput*  midiInput  = nullptr;
    MidiOutput* midiOutput = nullptr;

    std::vector<AudioInput*>     audioInputs;
    std::vector<AudioOutput*>    audioOutputs;
    std::vector<ParameterInput*> parameterInputs;

    std::vector<DspNode*> order;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DspProgram)
};


}  // namespace foleys::dsp
