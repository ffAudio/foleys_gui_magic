//
// Created by Daniel Walz on 25.02.24.
//

#pragma once

#include "../DSP/foleys_DspNode.h"

namespace foleys::dsp
{

class Analyser : public DspNode
{
public:
    Analyser (DspProgram& program, const juce::ValueTree& config);
    [[nodiscard]] juce::String getCategory() const override { return Category::Visualisers; }

    void prepare (juce::dsp::ProcessSpec spec) override;
    void process ([[maybe_unused]] int numSamples) override;

    FOLEYS_DECLARE_DSP_FACTORY (Analyser)

private:
    MagicAnalyser* analyser = nullptr;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Analyser)
};


}  // namespace foleys::dsp
