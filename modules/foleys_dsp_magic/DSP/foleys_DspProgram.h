//
// Created by Daniel Walz on 04.02.24.
//

#pragma once

#include "foleys_BuiltinNodes.h"

#include <juce_dsp/juce_dsp.h>

namespace foleys::dsp
{

class DspProgram
{
public:
    explicit DspProgram (MagicDspBuilder& builder);
    DspProgram (MagicDspBuilder& builder, const juce::ValueTree& tree);

    bool addNode (const juce::ValueTree& newNode);
    bool createNode (const juce::ValueTree& newNode);

    bool connectNodes (Connection::ConnectionType connectionType, int sourceUID, int sourceIndex, int targetUID, int targetIndex);
    void disconnect (int nodeUID, Connection::ConnectionType connectionType, int connectorIndex, bool input);

    void prepareToPlay (double sampleRate, int expectedNumSamples);
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi);
    void releaseResources();

    bool acceptsMidi() const { return midiInput != nullptr; }
    bool producesMidi() const { return midiOutput != nullptr; }
    bool isMidiEffect() const { return acceptsMidi() && producesMidi(); }

    DspNode* getNodeWithUID (int uid);

    std::vector<std::unique_ptr<DspNode>>::const_iterator begin() { return nodes.cbegin(); }
    std::vector<std::unique_ptr<DspNode>>::const_iterator end() { return nodes.cend(); }

    juce::ValueTree getConfig() const { return dspConfig; }

private:
    MagicDspBuilder& dspBuilder;
    juce::ValueTree  dspConfig { "Program" };

    std::vector<std::unique_ptr<DspNode>> nodes;
    std::map<int, DspNode*>               nodeLookup;
    int                                   uidCounter = 0;

    juce::WeakReference<DspNode> midiInput;
    juce::WeakReference<DspNode> midiOutput;

    std::vector<juce::WeakReference<DspNode>> audioInputs;
    std::vector<juce::WeakReference<DspNode>> audioOutputs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DspProgram)
};


}  // namespace foleys::dsp
