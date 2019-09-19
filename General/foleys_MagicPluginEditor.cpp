


namespace foleys
{

MagicPluginEditor::MagicPluginEditor (juce::AudioProcessor& processorToUse)
  : juce::AudioProcessorEditor (processorToUse),
    processor (processorToUse)
{
    builder.registerJUCEFactories();
    builder.registerJUCELookAndFeels();

    setSize (600, 400);
}

void MagicPluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
    g.drawFittedText ("Nothing to see yet", getLocalBounds().withSizeKeepingCentre (200, 20), juce::Justification::centred, 1);
}

void MagicPluginEditor::resized()
{

}

}
