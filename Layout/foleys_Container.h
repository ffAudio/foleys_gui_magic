
#pragma once

namespace foleys
{

class Container : public Decorator
{
public:
    enum class Layout
    {
        Stacked,
        VerticalBox,
        HorizontalBox,
        FlexBox,
        Grid
    };

    Container() = default;

    void addChild (std::unique_ptr<Decorator> child);

    void resized() override;

private:

    std::vector<std::unique_ptr<Decorator>> children;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Container)
};

} // namespace foleys
