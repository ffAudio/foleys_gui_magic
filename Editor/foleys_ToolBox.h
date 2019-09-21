
#pragma once

namespace foleys
{

class ToolBox  : public juce::Component,
                 private juce::Timer
{
public:
    ToolBox (juce::Component* parent);

    void paint (juce::Graphics& g) override;

    void timerCallback () override;

private:
    juce::Component::SafePointer<juce::Component> parent;
    juce::Point<int> parentPos;
    int              parentHeight = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToolBox)
};

} // namespace foleys
