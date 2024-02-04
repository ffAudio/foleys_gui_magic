//
// Created by Daniel Walz on 04.02.24.
//

#pragma once

#include "foleys_DspNode.h"


namespace foleys
{

class MagicDspBuilder;

class AudioInput : public DspNode
{
public:
    AudioInput (MagicDspBuilder& builder, const juce::ValueTree& node) : DspNode (node) { }

    FOLEYS_DECLARE_DSP_FACTORY (AudioInput)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioInput)
};

class AudioOutput : public DspNode
{
public:
    AudioOutput (MagicDspBuilder& builder, const juce::ValueTree& node) : DspNode (node) { }

    FOLEYS_DECLARE_DSP_FACTORY (AudioOutput)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioOutput)
};

class MidiInput : public DspNode
{
public:
    MidiInput (MagicDspBuilder& builder, const juce::ValueTree& node) : DspNode (node) { }

    FOLEYS_DECLARE_DSP_FACTORY (MidiInput)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiInput)
};

class MidiOutput : public DspNode
{
public:
    MidiOutput (MagicDspBuilder& builder, const juce::ValueTree& node) : DspNode (node) { }

    FOLEYS_DECLARE_DSP_FACTORY (MidiOutput)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiOutput)
};


}
