
namespace foleys
{

template <class AppType>
MagicGUIBuilder<AppType>::MagicGUIBuilder (juce::Component& parentToUse)
  : parent (parentToUse)
{
}

template <class AppType>
void MagicGUIBuilder<AppType>::restoreGUI (const juce::ValueTree& gui)
{
    // TODO: recalculate the layout of the components
}

template <class AppType>
void MagicGUIBuilder<AppType>::updateLayout ()
{

}

template <class AppType>
void MagicGUIBuilder<AppType>::registerFactory (juce::String type, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&, AppType&)> factory)
{
    if (factories.find (type) != factories.cend())
    {
        // You tried to add two factories with the same type name!
        // That cannot work, the second factory will be ignored.
        jassertfalse;
        return;
    }

    factories [type] = factory;
}

template <>
void MagicGUIBuilder<juce::AudioProcessor>::registerJUCEFactories()
{
    registerFactory ("Slider",
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         return std::make_unique<juce::Slider>();
                     });

    registerFactory ("ComboBox",
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         return std::make_unique<juce::ComboBox>();
                     });
}

template <>
void MagicGUIBuilder<juce::AudioProcessor>::registerLookAndFeel (juce::String name, std::unique_ptr<juce::LookAndFeel> lookAndFeel)
{
    if (lookAndFeels.find (name) != lookAndFeels.cend())
    {
        // You tried to register more than one LookAndFeel with the same name!
        // That cannot work, the second LookAndFeel will be ignored
        jassertfalse;
        return;
    }

    lookAndFeels [name] = std::move (lookAndFeel);
}

template <>
void MagicGUIBuilder<juce::AudioProcessor>::registerJUCELookAndFeels()
{
    registerLookAndFeel ("LookAndFeel_V1", std::make_unique<juce::LookAndFeel_V1>());
    registerLookAndFeel ("LookAndFeel_V2", std::make_unique<juce::LookAndFeel_V2>());
    registerLookAndFeel ("LookAndFeel_V3", std::make_unique<juce::LookAndFeel_V3>());
    registerLookAndFeel ("LookAndFeel_V4", std::make_unique<juce::LookAndFeel_V4>());

}

}
