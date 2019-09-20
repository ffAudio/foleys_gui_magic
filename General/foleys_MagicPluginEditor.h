
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

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    MagicProcessorState& processorState;

    MagicGUIBuilder<juce::AudioProcessor> builder { *this };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicPluginEditor)
};

} // namespace foleys
