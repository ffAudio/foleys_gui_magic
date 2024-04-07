//
// Created by Daniel Walz on 04.02.24.
//

#pragma once

#include "../DSP/foleys_MagicDspBuilder.h"

#include <foleys_gui_magic/foleys_gui_magic.h>

namespace foleys::dsp
{

/**
 * The PluginProcessor is an entire plugin in a XML ValueTree
 */
class PluginProcessor : public MagicProcessor
{
public:
    PluginProcessor();

    /**
     * Create this processor from the BinaryData
     * @param magic
     * @param magic_size
     */
    PluginProcessor (const char* magic, size_t magic_size);

    /**
     * Set the whole config consisting of parameters, DSP chain and GUI
     * @param mainConfig
     */
    void                          setValueTree (const juce::ValueTree& mainConfig);
    [[nodiscard]] juce::ValueTree getValueTree();

    const juce::String getName() const override { return m_name; }

    void prepareToPlay (double sampleRate, int expectedNumSamples) override;
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;
    void releaseResources() override;

    // ================================================================================

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;

    MagicDspBuilder& getMagicDspBuilder() { return m_magicDspBuilder; }

private:
    MagicDspBuilder m_magicDspBuilder { magicState };

    juce::ValueTree             m_config;
    juce::String                m_name { "Plugin-Gui-Magic" };
    juce::CriticalSection       m_programLock;
    std::unique_ptr<DspProgram> m_currentProgram;

    double sampleRate         = 0.0;
    int    expectedNumSamples = 0;
    bool   hasMidiIn          = false;
    bool   hasMidiOut         = false;
    bool   midiEffect         = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};

}  // namespace foleys::dsp
