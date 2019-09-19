
#pragma once

namespace foleys
{

class Stylesheet
{
public:

    struct Class
    {


    };

    Stylesheet() = default;
    Stylesheet (juce::ValueTree config, juce::UndoManager* undo = nullptr);

    /**
     Read the DOM and style classes from the ValueTree

     @param config is the ValueTree containing the whole GUI
     */
    void readFromValueTree (juce::ValueTree config, juce::UndoManager* undo);

    /**
     This method traverses the dom and checks each style, if that property was defined.

     @param name the name of the property
     */
    juce::var getProperty (const juce::String& name);

private:

    juce::ValueTree config;

    std::map<juce::String, Class> classes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Stylesheet)
};

} // namespace foleys
