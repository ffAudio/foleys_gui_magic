
#pragma once

namespace foleys
{

class MagicProcessorState
{
public:
    MagicProcessorState (juce::AudioProcessor& processorToUse,
                         juce::AudioProcessorValueTreeState& stateToUse);

    juce::AudioProcessor& getProcessor();
    juce::AudioProcessorValueTreeState& getValueTreeState();

private:

    juce::AudioProcessor& processor;
    juce::AudioProcessorValueTreeState& state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicProcessorState)
};

} // namespace foleys
