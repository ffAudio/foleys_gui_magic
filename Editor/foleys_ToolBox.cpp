
namespace foleys
{

ToolBox::ToolBox (juce::Component* parentToUse)
  : parent (parentToUse)
{
    setOpaque (true);
    setBounds (100, 100, 300, 700);
    addToDesktop (getLookAndFeel().getMenuWindowFlags());

    startTimer (100);

    setVisible (true);
}

void ToolBox::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
    g.setColour (juce::Colours::silver);
    g.drawRect (getLocalBounds().toFloat(), 2.0f);
    g.drawFittedText ("foleys GUI Magic", getLocalBounds().withHeight (24), juce::Justification::centred, 1);
}

void ToolBox::timerCallback ()
{
    if (parent == nullptr)
        return;

    const auto pos = parent->getScreenPosition();
    const auto height = parent->getHeight();
    if (pos != parentPos || height != parentHeight)
    {
        const auto width = 200;
        parentPos = pos;
        parentHeight = height;
        setBounds (parentPos.getX() - width, parentPos.getY(),
                   width,                    parentHeight * 0.9f);
    }
}



} // namespace foleys
