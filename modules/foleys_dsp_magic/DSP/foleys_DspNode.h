//
// Created by Daniel Walz on 04.02.24.
//

#pragma once

namespace foleys::dsp
{

class MagicDspBuilder;

class DspNode
{
public:
    explicit DspNode (MagicDspBuilder& builder, const juce::ValueTree& node);
    virtual ~DspNode();

    [[nodiscard]] juce::String getName() const { return nodeName; }
    [[nodiscard]] juce::String getType() const { return nodeType; }
    [[nodiscard]] int          getUID() const { return uid; }

    virtual void prepare (juce::dsp::ProcessSpec spec)                                  = 0;
    virtual void process (juce::dsp::AudioBlock<float>& buffer, juce::MidiBuffer& midi) = 0;
    virtual void release()                                                              = 0;

    [[nodiscard]] const juce::ValueTree& getConfig() const { return config; }

private:
    juce::ValueTree config;
    juce::String    nodeName;
    juce::String    nodeType;
    int             uid = 0;

    JUCE_DECLARE_WEAK_REFERENCEABLE (DspNode)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DspNode)
};

#define FOLEYS_DECLARE_DSP_FACTORY(itemName)                                                                                                                   \
    static inline std::unique_ptr<DspNode> factory (foleys::dsp::MagicDspBuilder& builder, const juce::ValueTree& node)                                        \
    {                                                                                                                                                          \
        return std::make_unique<itemName> (builder, node);                                                                                                     \
    }                                                                                                                                                          \
    static constexpr auto ID = #itemName;

}  // namespace foleys::dsp
