//
// Created by Daniel Walz on 04.02.24.
//

#pragma once

#include "foleys_dsp_magic/DSP/foleys_DspNode.h"


namespace foleys::dsp
{

class MagicDspBuilder;

class AudioInput : public DspNode
{
public:
    AudioInput (DspProgram& program, const juce::ValueTree& node);
    [[nodiscard]] juce::String getCategory() const override { return Category::InOut; }

    void prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) override { }
    void process ([[maybe_unused]] int numSamples) override { }

    void setAudioBuffer (float* const* data, int numChannels, int numSamples);

    FOLEYS_DECLARE_DSP_FACTORY (AudioInput)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioInput)
};

class AudioOutput : public DspNode
{
public:
    AudioOutput (DspProgram& program, const juce::ValueTree& node);
    [[nodiscard]] juce::String getCategory() const override { return Category::InOut; }

    void prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) override { }
    void process ([[maybe_unused]] int numSamples) override { }

    FOLEYS_DECLARE_DSP_FACTORY (AudioOutput)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioOutput)
};

class ParameterInput : public DspNode
{
public:
    ParameterInput (DspProgram& program, const juce::ValueTree& node);
    [[nodiscard]] juce::String getCategory() const override { return Category::InOut; }

    void prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) override { }
    void process (int numSamples) override;

    void setParameterGroup (const juce::AudioProcessorParameterGroup& group);

    FOLEYS_DECLARE_DSP_FACTORY (ParameterInput)
private:
    std::vector<juce::RangedAudioParameter*> parameters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterInput)
};

class MidiInput : public DspNode
{
public:
    MidiInput (DspProgram& program, const juce::ValueTree& node) : DspNode (program, node) { }
    [[nodiscard]] juce::String getCategory() const override { return Category::InOut; }

    void prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) override { }
    void process ([[maybe_unused]] int numSamples) override { }

    [[nodiscard]] bool hasMidiOutput() const override { return true; }

    FOLEYS_DECLARE_DSP_FACTORY (MidiInput)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiInput)
};

class MidiOutput : public DspNode
{
public:
    MidiOutput (DspProgram& program, const juce::ValueTree& node) : DspNode (program, node) { }
    [[nodiscard]] juce::String getCategory() const override { return Category::InOut; }

    void prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) override { }
    void process ([[maybe_unused]] int numSamples) override { }

    [[nodiscard]] bool hasMidiInput() const override { return true; }

    FOLEYS_DECLARE_DSP_FACTORY (MidiOutput)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiOutput)
};


}  // namespace foleys::dsp
