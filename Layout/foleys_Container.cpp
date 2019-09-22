

namespace foleys
{

void Container::addChild (std::unique_ptr<Decorator> child)
{
    children.push_back (std::move (child));
    addAndMakeVisible (child.get());
}

void Container::resized()
{
    if (children.empty())
        return;

    auto bounds = getClientBounds();
    int w = bounds.getWidth() / children.size();

    for (auto& child : children)
        child->setBounds (bounds.removeFromLeft (w));
}

} // namespace foleys
