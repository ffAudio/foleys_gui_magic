//
// Created by Daniel Walz on 04.02.24.
//

#pragma once

#include "foleys_Connection.h"

namespace foleys::dsp
{

class DspProgram;

namespace Category
{
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

    virtual void prepare (juce::dsp::ProcessSpec spec)                                  = 0;
    virtual void process (juce::dsp::AudioBlock<float>& buffer, juce::MidiBuffer& midi) = 0;
    virtual void release()                                                              = 0;

    void updateConnections();

    [[nodiscard]] virtual int  getNumAudioInputs() const { return 0; }
    [[nodiscard]] virtual int  getNumAudioOutputs() const { return 0; }
    [[nodiscard]] virtual int  getNumParameterInputs() const { return 0; }
    [[nodiscard]] virtual int  getNumParameterOutputs() const { return 0; }
    [[nodiscard]] virtual bool hasMidiInput() const { return false; }
    [[nodiscard]] virtual bool hasMidiOutput() const { return false; }

    [[nodiscard]] virtual juce::String getAudioInputName (int index) const { return TRANS ("Audio ") + juce::String (index); }
    [[nodiscard]] virtual juce::String getAudioOutputName (int index) const { return TRANS ("Audio ") + juce::String (index); }
    [[nodiscard]] virtual juce::String getParameterInputName (int index) const { return TRANS ("Parameter ") + juce::String (index); }
    [[nodiscard]] virtual juce::String getParameterOutputName (int index) const { return TRANS ("Parameter ") + juce::String (index); }
    [[nodiscard]] virtual juce::String getMidiInputName() const { return TRANS ("Midi In "); }
    [[nodiscard]] virtual juce::String getMidiOutputName() const { return TRANS ("Midi Out "); }

    const Connection* getConnection (Connection::ConnectionType type, int index) const;
    Connection*       getConnection (Connection::ConnectionType type, int index);

    [[nodiscard]] const juce::ValueTree& getConfig() const { return config; }

    static int getUID (const juce::ValueTree& tree);

    DspProgram& getProgram() { return program; }

private:
    DspProgram&     program;
    juce::ValueTree config;
    juce::String    nodeType;

    Connection midiInput { *this, Connection::MIDI };

    std::vector<Connection> audioInputs;
    std::vector<Connection> parameterInputs;


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
