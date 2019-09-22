

namespace foleys
{

Decorator::Decorator()
{
    setOpaque (margin == 0 && radius == 0);
}

Decorator::Decorator (std::unique_ptr<juce::Component> wrapped)
  : component (std::move (wrapped))
{
    setOpaque (margin == 0 && radius == 0);

    if (component.get() != nullptr)
        addAndMakeVisible (component.get());
}

void Decorator::setEditable (bool shouldEdit)
{
    setInterceptsMouseClicks (shouldEdit, true);

    if (component.get() != nullptr)
        component->setInterceptsMouseClicks (!shouldEdit, true);
}

void Decorator::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat().reduced (margin);
    // TODO: read from stylesheet
    auto parentBackground = juce::Colours::grey;
    auto background = juce::Colours::magenta;
    auto borderColour = juce::Colours::red;

    g.fillAll (parentBackground);

    if (radius > 0.0f)
    {
        g.setColour (background);
        g.fillRoundedRectangle (bounds, radius);

        if (border > 0.0f)
        {
            g.setColour (borderColour);
            g.drawRoundedRectangle (bounds, radius, border);
        }
    }
    else
    {
        g.setColour (background);
        g.fillRect (bounds);

        if (border > 0.0f)
        {
            g.setColour (borderColour);
            g.drawRect (bounds, border);
        }
    }


}

juce::Rectangle<int> Decorator::getClientBounds() const
{
    return getLocalBounds().reduced (margin + padding);
}

void Decorator::resized()
{
    if (component.get() != nullptr)
        component->setBounds (getClientBounds());
}

}
