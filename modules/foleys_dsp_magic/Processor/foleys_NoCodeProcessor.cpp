//
// Created by Daniel Walz on 04.02.24.
//

#include "foleys_NoCodeProcessor.h"

namespace foleys
{


NoCodeProcessor::NoCodeProcessor (const char* magic, size_t magic_size)
{
    auto tree = juce::ValueTree::fromXml (juce::String (magic, magic_size));

    if (tree.isValid())
    {
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

void NoCodeProcessor::prepareToPlay (double sampleRate, int expectedNumSamples)
{
    juce::ScopedLock lock (m_programLock);
    if (m_currentProgram)
        m_currentProgram->prepareToPlay (sampleRate, expectedNumSamples);
}

void NoCodeProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedLock lock (m_programLock);
    if (!m_currentProgram)
    {
        buffer.clear();
        return;
    }

    m_currentProgram->processBlock (buffer, midi);
}

void NoCodeProcessor::releaseResources()
{
    juce::ScopedLock lock (m_programLock);
    if (m_currentProgram)
        m_currentProgram->releaseResources();
}

// ================================================================================

bool NoCodeProcessor::acceptsMidi() const
{
    juce::ScopedLock lock (m_programLock);
    return m_currentProgram ? m_currentProgram->acceptsMidi() : false;
}

bool NoCodeProcessor::producesMidi() const
{
    juce::ScopedLock lock (m_programLock);
    return m_currentProgram ? m_currentProgram->producesMidi() : false;
}

bool NoCodeProcessor::isMidiEffect() const
{
    juce::ScopedLock lock (m_programLock);
    return m_currentProgram ? m_currentProgram->isMidiEffect() : false;
}


}  // namespace foleys
