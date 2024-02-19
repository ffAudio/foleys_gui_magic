//
// Created by Daniel Walz on 04.02.24.
//

#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>

#include "../DSP/foleys_MagicDspBuilder.h"

namespace foleys
{

/**
 * The PluginProcessor is an entire plugin in a XML ValueTree
 */
class PluginProcessor : public MagicProcessor
{
public:
    /**
     * Create this processor from the BinaryData
     * @param magic
     * @param magic_size
     */
    PluginProcessor (const char* magic, size_t magic_size);

    void prepareToPlay (double sampleRate, int expectedNumSamples) override;
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;
    void releaseResources() override;

    // ================================================================================

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;

private:
    juce::CriticalSection m_programLock;
    MagicDspBuilder       m_magicDspBuilder { magicState };

    std::unique_ptr<DspProgram> m_currentProgram;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};

}  // namespace foleys
