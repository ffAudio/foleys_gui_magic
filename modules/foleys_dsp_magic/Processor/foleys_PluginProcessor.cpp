//
// Created by Daniel Walz on 04.02.24.
//

#include "foleys_PluginProcessor.h"

namespace foleys
{


PluginProcessor::PluginProcessor (const char* magic, size_t magic_size)
{
    auto tree = juce::ValueTree::fromXml (juce::String (magic, magic_size));

    if (tree.isValid())
    {
        if (tree.hasProperty (IDs::name))
            m_name = tree.getProperty (IDs::name);

        juce::AudioProcessorParameterGroup parameterTree;
        foleys::ParametersSerialisation::restoreParameterTree (&parameterTree, tree);
        setParameterTree (std::move (parameterTree));

        auto program = m_magicDspBuilder.createProgram (tree.getChildWithName ("DSP"));
        {
            juce::ScopedLock lock (m_programLock);
            m_currentProgram = std::move (program);
        }

        magicState.setGuiValueTree (tree);
    }
}

// ================================================================================

void PluginProcessor::prepareToPlay (double sampleRate, int expectedNumSamples)
{
    juce::ScopedLock lock (m_programLock);
    if (m_currentProgram)
        m_currentProgram->prepareToPlay (sampleRate, expectedNumSamples);
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedLock lock (m_programLock);
    if (!m_currentProgram)
    {
        buffer.clear();
        return;
    }

    m_currentProgram->processBlock (buffer, midi);
}

void PluginProcessor::releaseResources()
{
    juce::ScopedLock lock (m_programLock);
    if (m_currentProgram)
        m_currentProgram->releaseResources();
}

// ================================================================================

bool PluginProcessor::acceptsMidi() const
{
    juce::ScopedLock lock (m_programLock);
    return m_currentProgram ? m_currentProgram->acceptsMidi() : false;
}

bool PluginProcessor::producesMidi() const
{
    juce::ScopedLock lock (m_programLock);
    return m_currentProgram ? m_currentProgram->producesMidi() : false;
}

bool PluginProcessor::isMidiEffect() const
{
    juce::ScopedLock lock (m_programLock);
    return m_currentProgram ? m_currentProgram->isMidiEffect() : false;
}


}  // namespace foleys
