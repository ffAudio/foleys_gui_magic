//
// Created by Daniel Walz on 04.02.24.
//

#include "foleys_PluginProcessor.h"

namespace foleys::dsp
{

namespace IDs
{
static constexpr auto* name       = "name";
static constexpr auto* parameters = "Parameters";
}  // namespace IDs

PluginProcessor::PluginProcessor (const char* magic, size_t magic_size)
{
    auto tree = juce::ValueTree::fromXml (juce::String (magic, magic_size));

    setValueTree (tree);
}

// ================================================================================

void PluginProcessor::setValueTree (const juce::ValueTree& mainConfig)
{
    if (mainConfig.isValid())
    {
        if (mainConfig.hasProperty (IDs::name))
            m_name = mainConfig.getProperty (IDs::name);

        auto oldParameterNode = m_config.getChildWithName (IDs::parameters);
        if (!oldParameterNode.isValid() || !oldParameterNode.isEquivalentTo (mainConfig.getChildWithName (IDs::parameters)))
        {
            juce::AudioProcessorParameterGroup parameterTree;
            foleys::ParametersSerialisation::restoreParameterTree (&parameterTree, mainConfig);
            setParameterTree (std::move (parameterTree));
        }

        auto program = m_magicDspBuilder.createProgram (mainConfig.getChildWithName ("DSP"));
        {
            if (sampleRate > 0.0)
                program->prepareToPlay (sampleRate, expectedNumSamples);

            juce::ScopedLock lock (m_programLock);
            m_currentProgram = std::move (program);
        }

        magicState.setGuiValueTree (mainConfig);

        m_config = mainConfig;
    }
}

// ================================================================================

void PluginProcessor::prepareToPlay (double newSampleRate, int newExpectedNumSamples)
{
    sampleRate         = newSampleRate;
    expectedNumSamples = newExpectedNumSamples;

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


}  // namespace foleys::dsp
