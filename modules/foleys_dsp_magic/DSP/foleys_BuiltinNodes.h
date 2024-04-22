//
// Created by Daniel Walz on 04.02.24.
//

#pragma once

#include "foleys_DspNode.h"


namespace foleys::dsp
{

class MagicDspBuilder;

class AudioInput : public DspNode
{
public:
    AudioInput (DspProgram& program, const juce::ValueTree& node) : DspNode (program, node) { }
    [[nodiscard]] juce::String getCategory() const override { return Category::Audio; }

    void prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) override { }
    void process ([[maybe_unused]] juce::dsp::AudioBlock<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midi) override { }
    void release() override { }

    [[nodiscard]] int getNumAudioOutputs() const override { return 1; }

    FOLEYS_DECLARE_DSP_FACTORY (AudioInput)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioInput)
};

class AudioOutput : public DspNode
{
public:
    AudioOutput (DspProgram& program, const juce::ValueTree& node) : DspNode (program, node) { }
    [[nodiscard]] juce::String getCategory() const override { return Category::Audio; }

    void prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) override { }
    void process ([[maybe_unused]] juce::dsp::AudioBlock<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midi) override { }
    void release() override { }

    [[nodiscard]] int getNumAudioInputs() const override { return 1; }

    FOLEYS_DECLARE_DSP_FACTORY (AudioOutput)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioOutput)
};

class MidiInput : public DspNode
{
public:
    MidiInput (DspProgram& program, const juce::ValueTree& node) : DspNode (program, node) { }
    [[nodiscard]] juce::String getCategory() const override { return Category::Audio; }

    void prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) override { }
    void process ([[maybe_unused]] juce::dsp::AudioBlock<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midi) override { }
    void release() override { }

    [[nodiscard]] bool hasMidiOutput() const override { return true; }

    FOLEYS_DECLARE_DSP_FACTORY (MidiInput)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiInput)
};

class MidiOutput : public DspNode
{
public:
    MidiOutput (DspProgram& program, const juce::ValueTree& node) : DspNode (program, node) { }
    [[nodiscard]] juce::String getCategory() const override { return Category::Audio; }

    void prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) override { }
    void process ([[maybe_unused]] juce::dsp::AudioBlock<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midi) override { }
    void release() override { }

    [[nodiscard]] bool hasMidiInput() const override { return true; }

    FOLEYS_DECLARE_DSP_FACTORY (MidiOutput)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiOutput)
};


}  // namespace foleys::dsp
