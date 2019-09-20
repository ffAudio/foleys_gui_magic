


namespace foleys
{

MagicPluginEditor::MagicPluginEditor (MagicProcessorState& stateToUse)
  : juce::AudioProcessorEditor (stateToUse.getProcessor()),
    processorState (stateToUse)
{
    builder.registerJUCEFactories();
    builder.registerJUCELookAndFeels();

    setSize (600, 400);
}

void MagicPluginEditor::restoreGUI (const juce::ValueTree& gui)
{
    builder.restoreGUI (gui);
}

void MagicPluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
    g.drawFittedText ("Nothing to see yet", getLocalBounds().withSizeKeepingCentre (300, 30), juce::Justification::centred, 1);
}

void MagicPluginEditor::resized()
{
    builder.updateLayout();
}

void MagicPluginEditor::restoreGUI (const char* data, const int dataSize)
{
    juce::String text (data, dataSize);
    auto gui = juce::ValueTree::fromXml (text);
    restoreGUI (gui);
}

}
