
namespace foleys
{

Stylesheet::Stylesheet (juce::ValueTree configToUse, juce::UndoManager* undo)
{
    readFromValueTree (configToUse, undo);
}

void Stylesheet::readFromValueTree (juce::ValueTree configToUse, juce::UndoManager* undo)
{
    config = configToUse;

    classes.clear();

    auto styleNode = config.getOrCreateChildWithName ("Style", undo);
    if (styleNode.isValid())
    {

    }
}

juce::var Stylesheet::getProperty (const juce::String& name)
{
    return {};
}

} // namespace foleys
