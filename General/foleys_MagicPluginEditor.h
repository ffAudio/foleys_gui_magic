
#pragma once

namespace foleys
{

/**
 This is a generic AudioProcessorEditor, that is completely
 defined and styled by drag and drop. There is an XML representation,
 that is baked into the project using the BinaryData of Projucer.
 */
class MagicPluginEditor  : public juce::AudioProcessorEditor,
                           public juce::DragAndDropContainer
{
public:
    MagicPluginEditor (MagicProcessorState& processorState);

    /**
     Setup a GUI from a previously stored ValueTree

     @param gui the ValueTree that defines the GUI of the editor
     */
    void restoreGUI (const juce::ValueTree& gui);

    /**
     Setup a GUI from a previously stored ValueTree. This is the
     usual way to deploy your finished GUI via the Projucer:

     @code{.cpp}
     AudioProcessorEditor* MyAudioProcessor::createEditor()
     {
         auto* editor = new foleys::MagicPluginEditor (magicState);
         editor->restoreGUI (BinaryData::magic_xml, BinaryData::magic_xmlSize);
         return editor;
     }
     @endcode

     @param data points to the binary data of the XML file
     @param dataSize the number of bytes
     */
    void restoreGUI (const char* data, const int dataSize);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    MagicProcessorState& processorState;

    MagicGUIBuilder<juce::AudioProcessor> builder { *this };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicPluginEditor)
};

} // namespace foleys
