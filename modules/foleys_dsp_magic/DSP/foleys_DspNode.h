//
// Created by Daniel Walz on 04.02.24.
//

#pragma once

#include "foleys_Connection.h"

#include <juce_dsp/juce_dsp.h>

namespace foleys::dsp
{

class DspProgram;

namespace Category
{
static constexpr auto InOut       = "Input/Output";
static constexpr auto Audio       = "Audio";
static constexpr auto Parameters  = "Parameters";
static constexpr auto Generators  = "Generators";
static constexpr auto Visualisers = "Visualisers";
static constexpr auto Filters     = "Filters";
static constexpr auto Scripts     = "Scripts";
}  // namespace Category


class DspNode
{
public:
    explicit DspNode (DspProgram& program, const juce::ValueTree& node);
    virtual ~DspNode();

    [[nodiscard]] virtual juce::String getCategory() const = 0;

    [[nodiscard]] juce::String getType() const { return nodeType; }
    [[nodiscard]] juce::String getName() const;
    [[nodiscard]] int          getUID() const;

    void setName (const juce::String& newName);
    void setUID (int newUID);

    virtual void prepare (juce::dsp::ProcessSpec spec) = 0;
    virtual void process()                             = 0;
    virtual void release() { }

    void updateConnections();

    [[nodiscard]] int          getNumAudioInputs() const;
    [[nodiscard]] int          getNumParameterInputs() const;
    [[nodiscard]] juce::String getAudioInputName (int index) const;
    [[nodiscard]] juce::String getParameterInputName (int index) const;
    [[nodiscard]] juce::String getMidiInputName() const;

    [[nodiscard]] int          getNumAudioOutputs() const;
    [[nodiscard]] int          getNumParameterOutputs() const;
    [[nodiscard]] juce::String getAudioOutputName (int index) const;
    [[nodiscard]] juce::String getParameterOutputName (int index) const;
    [[nodiscard]] juce::String getMidiOutputName() const;

    [[nodiscard]] virtual bool hasMidiInput() const { return false; }
    [[nodiscard]] virtual bool hasMidiOutput() const { return false; }

    void clearInputs (ConnectionType type);
    void clearOutputs (ConnectionType type);

    [[nodiscard]] Connection* getConnection (ConnectionType type, int index);

    /**
     * This is where the node delivers it's output
     * @param type  the type of requested output
     * @param index the index of the requested output
     * @return the output instance, can be nullptr if that output doesn't exist
     */
    Output* getOutput ([[maybe_unused]] ConnectionType type, [[maybe_unused]] int index);

    /**
     * Returns the output of the node this input is feeding from
     * @param type the type of the requested output
     * @param inputIndex the input index the output is connected to
     * @return An output instance, can be nullptr if it is not connected
     */
    Output* getConnectedOutput (ConnectionType type, int inputIndex);

    [[nodiscard]] const juce::ValueTree& getConfig() const { return config; }

    static int getUID (const juce::ValueTree& tree);

    DspProgram& getProgram() { return program; }

    std::vector<DspNode*> getNodesToDependOn();

private:
    DspProgram&     program;
    juce::ValueTree config;
    juce::String    nodeType;

    std::vector<Connection> audioInputs;
    std::vector<Connection> parameterInputs;

    std::vector<Output> audioOutputs;
    std::vector<Output> parameterOutputs;

protected:
    void addAudioInput (const juce::String& name);
    void addParameterInput (const juce::String& name);

    void addAudioOutput (const juce::String& name);
    void addParameterOutput (const juce::String& name);

    const std::vector<Connection>& getAudioInputs() { return audioInputs; }
    const std::vector<Connection>& getParameterInputs() { return parameterInputs; }

    Connection midiInput { *this, ConnectionType::MIDI, TRANS ("MIDI in") };

    JUCE_DECLARE_WEAK_REFERENCEABLE (DspNode)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DspNode)
};

#define FOLEYS_DECLARE_DSP_FACTORY(itemName)                                                                                                                   \
    static inline std::unique_ptr<DspNode> factory (foleys::dsp::DspProgram& program, const juce::ValueTree& node)                                             \
    {                                                                                                                                                          \
        return std::make_unique<itemName> (program, node);                                                                                                     \
    }                                                                                                                                                          \
    static constexpr auto ID = #itemName;

}  // namespace foleys::dsp
