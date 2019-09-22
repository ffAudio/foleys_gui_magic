
#pragma once

namespace foleys
{

/**
 The MagicGUIBuilder is responsible to recreate the GUI from a single ValueTree.
 You can add your own factories to the builder to allow additional components.
 The template parameter AppType specifies a type, that each components gets a reference of,
 it is intended to be eithe AudioProcessor, JUCEApplication or your own MainWindow, it's up to you.
 */
template <class AppType>
class MagicGUIBuilder
{
public:

    MagicGUIBuilder (juce::Component& parent, AppType& app);

    /**
     Loads a gui from a previously stored ValueTree.
     */
    void restoreGUI (const juce::ValueTree& gui);

    /**
     Recalculates the layout of all components
     */
    void updateLayout ();

    void registerFactory (juce::String type, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&, AppType&)> factory);

    void registerJUCEFactories();

    void registerLookAndFeel (juce::String name, std::unique_ptr<juce::LookAndFeel> lookAndFeel);

    void registerJUCELookAndFeels();

    std::unique_ptr<Decorator> restoreNode (juce::Component& component, const juce::ValueTree& node);

private:

    juce::Component& parent;

    AppType&         app;

    std::map<juce::String, std::unique_ptr<juce::LookAndFeel>> lookAndFeels;

    std::map<juce::String, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&, AppType&)>> factories;

    juce::UndoManager undo;
    juce::ValueTree   config;
    Stylesheet stylesheet;

    std::unique_ptr<Decorator> root;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicGUIBuilder)
};

} // namespace foleys
