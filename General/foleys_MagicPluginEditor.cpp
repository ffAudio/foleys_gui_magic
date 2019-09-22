


namespace foleys
{

MagicPluginEditor::MagicPluginEditor (MagicProcessorState& stateToUse)
  : juce::AudioProcessorEditor (stateToUse.getProcessor()),
    processorState (stateToUse)
{
    builder.registerJUCEFactories();
    builder.registerJUCELookAndFeels();

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    magicToolBox = std::make_unique<ToolBox>(getTopLevelComponent());
#endif

    setResizable (true, true);

    setSize (600, 400);
}

void MagicPluginEditor::restoreGUI (const juce::ValueTree& gui)
{
    builder.restoreGUI (gui);
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
