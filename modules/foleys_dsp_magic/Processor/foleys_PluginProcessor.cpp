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

PluginProcessor::PluginProcessor()
{
    auto tree = MagicProcessor::createGuiValueTree();
    tree.appendChild (juce::ValueTree ("Parameters"), nullptr);
    tree.appendChild (juce::ValueTree ("DSP", {}, { juce::ValueTree ("Program") }), nullptr);

    setValueTree (tree);
}

PluginProcessor::PluginProcessor (const char* magic, size_t magic_size)
{
    m_magicDspBuilder.registerBuiltinFactories();

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
            foleys::dsp::Parameters::restoreParameterTree (&parameterTree, mainConfig);
            setParameterTree (std::move (parameterTree));

            updateHostDisplay (juce::AudioProcessor::ChangeDetails().withParameterInfoChanged (true));
            magicState.updateParameterMap();
        }

        auto program = m_magicDspBuilder.createProgram (mainConfig.getChildWithName ("DSP").getChildWithName ("Program"));
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

juce::ValueTree PluginProcessor::getValueTree()
{
    return m_config;
}

// ================================================================================

void PluginProcessor::prepareToPlay (double newSampleRate, int newExpectedNumSamples)
{
    sampleRate         = newSampleRate;
    expectedNumSamples = newExpectedNumSamples;

    juce::ScopedLock lock (m_programLock);
    hasMidiIn  = m_currentProgram && m_currentProgram->acceptsMidi();
    hasMidiOut = m_currentProgram && m_currentProgram->producesMidi();
    midiEffect = m_currentProgram && m_currentProgram->isMidiEffect();

    if (m_currentProgram)
        m_currentProgram->prepareToPlay (sampleRate, expectedNumSamples);
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    juce::ScopedLock lock (m_programLock);
    if (!m_currentProgram)
        return;

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
    return hasMidiIn;
}

bool PluginProcessor::producesMidi() const
{
    return hasMidiOut;
}

bool PluginProcessor::isMidiEffect() const
{
    return midiEffect;
}


}  // namespace foleys::dsp
