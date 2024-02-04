//
// Created by Daniel Walz on 04.02.24.
//

#pragma once

namespace foleys
{

class DspNode
{
public:
    explicit DspNode(const juce::ValueTree& node);
    virtual ~DspNode();



private:
    JUCE_DECLARE_WEAK_REFERENCEABLE(DspNode)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DspNode)
};

#define FOLEYS_DECLARE_DSP_FACTORY(itemName) \
static inline std::unique_ptr<DspNode> factory (foleys::MagicDspBuilder& builder, const juce::ValueTree& node)\
{\
    return std::make_unique<itemName>(builder, node);\
}                                            \
static constexpr auto ID=#itemName;

}

