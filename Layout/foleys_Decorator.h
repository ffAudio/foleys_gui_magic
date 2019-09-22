
#pragma once

namespace foleys
{

/**
 The Decorator class will draw borders and descriptions around widgets, if defined.
 It also owns the Component and the Attachment, in case the Component is connected
 to an AudioProcessorValueTreeState.
 */
class Decorator : public juce::Component
{
public:
    Decorator();
    Decorator (std::unique_ptr<juce::Component> wrapped);

    /**
     This method sets the GUI in edit mode, that allows to drag the components around.
     */
    void setEditable (bool shouldEdit);

    void paint (juce::Graphics& g) override;
    void resized() override;

protected:

    juce::Rectangle<int> getClientBounds() const;

private:

    std::unique_ptr<juce::Component> component;

    float margin  = 3.0f;
    float padding = 5.0f;
    float border  = 2.0f;
    float radius  = 5.0f;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboboxAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Decorator)
};

} // namespace foleys
