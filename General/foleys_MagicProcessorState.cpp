

namespace foleys
{

MagicProcessorState::MagicProcessorState (juce::AudioProcessor& processorToUse,
                                          juce::AudioProcessorValueTreeState& stateToUse)
  : processor (processorToUse),
    state (stateToUse)
{

}

juce::AudioProcessor& MagicProcessorState::getProcessor()
{
    return processor;
}

juce::AudioProcessorValueTreeState& MagicProcessorState::getValueTreeState()
{
    return state;
}

} // namespace foleys
