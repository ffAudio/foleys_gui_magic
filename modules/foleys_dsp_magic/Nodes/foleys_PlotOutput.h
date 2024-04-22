//
// Created by Daniel Walz on 25.02.24.
//

#pragma once

#include "../DSP/foleys_DspNode.h"

namespace foleys::dsp
{

class PlotOutput : public DspNode
{
public:
    PlotOutput (DspProgram& program, const juce::ValueTree& config);
    [[nodiscard]] juce::String getCategory() const override { return Category::Visualisers; }

    void prepare (juce::dsp::ProcessSpec spec) override { }
    void process (juce::dsp::AudioBlock<float>& buffer, juce::MidiBuffer& midi) override { }
    void release() override { }

    [[nodiscard]] int getNumAudioInputs() const override { return 1; }

    FOLEYS_DECLARE_DSP_FACTORY (PlotOutput)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlotOutput)
};


}  // namespace foleys::dsp
