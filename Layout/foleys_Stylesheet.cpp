/*
 ==============================================================================
    Copyright (c) 2019 Foleys Finest Audio Ltd. - Daniel Walz
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.
 ==============================================================================
 */

#if FOLEYS_ENABLE_BINARY_DATA
namespace BinaryData
{
    // Number of elements in the namedResourceList and originalFileNames arrays.
    extern const int namedResourceListSize;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    extern const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);
}
#endif

namespace foleys
{

namespace IDs
{
    static juce::Identifier nodes     { "Nodes"   };
    static juce::Identifier classes   { "Classes" };
    static juce::Identifier types     { "Types"   };

    static juce::Identifier backgroundImage     { "background-image" };
    static juce::String     linearGradient      { "linear-gradient" };
    static juce::String     radialGradient      { "radial-gradient" };
    static juce::String     repeatingLinearGradient { "repeating-linear-gradient" };
    static juce::String     repeatingRadialGradient { "repeating-radial-gradient" };

    static juce::Identifier flexDirection       { "flex-direction" };
    static juce::String     flexDirRow          { "row" };
    static juce::String     flexDirRowReverse   { "row-reverse" };
    static juce::String     flexDirColumn       { "column" };
    static juce::String     flexDirColumnReverse { "column-reverse" };

    static juce::Identifier flexWrap            { "flex-wrap" };
    static juce::String     flexNoWrap          { "nowrap" };
    static juce::String     flexWrapNormal      { "wrap" };
    static juce::String     flexWrapReverse     { "wrap-reverse" };

    static juce::Identifier flexGrow            { "flex-grow" };
    static juce::Identifier flexShrink          { "flex-shrink" };

    static juce::Identifier flexAlignContent    { "flex-align-content" };
    static juce::Identifier flexAlignItems      { "flex-align-items" };
    static juce::Identifier flexJustifyContent  { "flex-justify-content" };
    static juce::Identifier flexAlignSelf       { "flex-align-self" };
    static juce::Identifier flexOrder           { "flex-order" };
    static juce::String     flexStretch         { "stretch" };
    static juce::String     flexStart           { "start" };
    static juce::String     flexEnd             { "end" };
    static juce::String     flexCenter          { "center" };
    static juce::String     flexSpaceBetween    { "space-between" };
    static juce::String     flexSpaceAround     { "space-around" };
    static juce::String     flexAuto            { "auto" };

    static juce::Identifier minWidth    { "min-width" };
    static juce::Identifier maxWidth    { "max-width" };
    static juce::Identifier minHeight   { "min-height" };
    static juce::Identifier maxHeight   { "max-height" };
    static juce::Identifier width       { "width" };
    static juce::Identifier height      { "height" };
}

void Stylesheet::setStyle (const juce::ValueTree& node)
{
    currentStyle = node;
}

juce::var Stylesheet::getProperty (const juce::Identifier& name, const juce::ValueTree& node, bool inherit) const
{
    if (inherit && node.hasProperty (IDs::id))
    {
        auto styleNode = currentStyle.getChildWithName (IDs::nodes);
        auto idNode = styleNode.getChildWithName (node.getProperty (IDs::id).toString());
        if (idNode.hasProperty (name))
            return idNode.getProperty (name);
    }

    auto classNames = node.getProperty (IDs::styleClass, {}).toString();
    for (auto className : juce::StringArray::fromTokens (classNames, " ", {}))
    {
        auto classesNode = currentStyle.getChildWithName (IDs::classes);
        auto classNode = classesNode.getChildWithName (className);
        if (classNode.hasProperty (name))
            return classNode.getProperty (name);
    }

    if (inherit)
    {
        auto typeNode = currentStyle.getChildWithName (IDs::types).getChildWithName (node.getType());
        if (typeNode.isValid() && typeNode.hasProperty (name))
            return typeNode.getProperty (name);
    }

    auto parent = node.getParent();
    if (parent.isValid() && parent.getType() != IDs::magic)
        return getProperty (name, parent, false);

    return {};
}

juce::Colour Stylesheet::parseColour (const juce::String& name) const
{
    return juce::Colours::findColourForName (name, juce::Colour::fromString (name.length() < 8 ? "ff" + name : name));
}

juce::LookAndFeel* Stylesheet::getLookAndFeel (const juce::ValueTree& node) const
{
    auto lnf = getProperty (IDs::lookAndFeel, node).toString();
    if (lnf.isNotEmpty())
    {
        const auto& it = lookAndFeels.find (lnf);
        if (it != lookAndFeels.end())
            return it->second.get();
    }

    return nullptr;
}

juce::Image Stylesheet::getBackgroundImage (const juce::ValueTree& node) const
{
#if FOLEYS_ENABLE_BINARY_DATA
    auto name = getProperty (IDs::backgroundImage, node);
    if (name.isVoid())
        return {};

    int dataSize = 0;
    const char* data = BinaryData::getNamedResource (name.toString().toRawUTF8(), dataSize);
    if (data != nullptr)
        return juce::ImageCache::getFromMemory (data, dataSize);

#endif
    return {};
}

juce::Array<juce::Colour> Stylesheet::getBackgroundGradient (const juce::ValueTree& node) const
{
    auto text = getProperty (IDs::backgroundImage, node).toString();

    if (text.startsWith (IDs::linearGradient))
    {
        auto parameters = getParameters (text);
        if (parameters.size() < 2)
            return {};

        juce::Array<juce::Colour> colours;
        for (int i=0; i < parameters.size(); ++i)
            colours.add (parseColour (parameters [i].trim()));

        return colours;
    }

    return {};
}

juce::StringArray Stylesheet::getParameters (const juce::String& text) const
{
    auto content = text.fromFirstOccurrenceOf ("(", false, true).upToFirstOccurrenceOf (")", false, true);
    return juce::StringArray::fromTokens (content, ",", {});
}

void Stylesheet::registerLookAndFeel (juce::String name, std::unique_ptr<juce::LookAndFeel> lookAndFeel)
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

void Stylesheet::configureFlexBox (juce::FlexBox& flexBox, const juce::ValueTree& node) const
{
    auto direction = getProperty (IDs::flexDirection, node).toString();
    if (direction == IDs::flexDirRow)
        flexBox.flexDirection = juce::FlexBox::Direction::row;
    else if (direction == IDs::flexDirRowReverse)
        flexBox.flexDirection = juce::FlexBox::Direction::rowReverse;
    else if (direction == IDs::flexDirColumn)
        flexBox.flexDirection = juce::FlexBox::Direction::column;
    else if (direction == IDs::flexDirColumnReverse)
        flexBox.flexDirection = juce::FlexBox::Direction::columnReverse;

    auto wrap = getProperty (IDs::flexWrap, node).toString();
    if (wrap == IDs::flexWrapNormal)
        flexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    else if (wrap == IDs::flexWrapReverse)
        flexBox.flexWrap = juce::FlexBox::Wrap::wrapReverse;
    else
        flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    auto alignContent = getProperty (IDs::flexAlignContent, node).toString();
    if (alignContent == IDs::flexStart)
        flexBox.alignContent = juce::FlexBox::AlignContent::flexStart;
    else if (alignContent == IDs::flexEnd)
        flexBox.alignContent = juce::FlexBox::AlignContent::flexEnd;
    else if (alignContent == IDs::flexCenter)
        flexBox.alignContent = juce::FlexBox::AlignContent::center;
    else if (alignContent == IDs::flexSpaceAround)
        flexBox.alignContent = juce::FlexBox::AlignContent::spaceAround;
    else if (alignContent == IDs::flexSpaceBetween)
        flexBox.alignContent = juce::FlexBox::AlignContent::spaceBetween;
    else
        flexBox.alignContent = juce::FlexBox::AlignContent::stretch;

    auto alignItems = getProperty (IDs::flexAlignItems, node).toString();
    if (alignItems == IDs::flexStart)
        flexBox.alignItems = juce::FlexBox::AlignItems::flexStart;
    else if (alignItems == IDs::flexEnd)
        flexBox.alignItems = juce::FlexBox::AlignItems::flexEnd;
    else if (alignItems == IDs::flexCenter)
        flexBox.alignItems = juce::FlexBox::AlignItems::center;
    else
        flexBox.alignItems = juce::FlexBox::AlignItems::stretch;

    auto justify = getProperty (IDs::flexJustifyContent, node).toString();
    if (justify == IDs::flexEnd)
        flexBox.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    else if (justify == IDs::flexCenter)
        flexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    else if (justify == IDs::flexSpaceAround)
        flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    else if (justify == IDs::flexSpaceBetween)
        flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    else
        flexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
}

void Stylesheet::configureFlexBoxItem (juce::FlexItem& item, const juce::ValueTree& node) const
{
    auto minWidth = getProperty (IDs::minWidth, node);
    if (! minWidth.isVoid())
        item.minWidth = minWidth;

    auto maxWidth = getProperty (IDs::maxWidth, node);
    if (! maxWidth.isVoid())
        item.maxWidth = maxWidth;

    auto minHeight = getProperty (IDs::minHeight, node);
    if (! minHeight.isVoid())
        item.minHeight = minHeight;

    auto maxHeight = getProperty (IDs::maxHeight, node);
    if (! maxHeight.isVoid())
        item.maxHeight = maxHeight;

    auto width = getProperty (IDs::width, node);
    if (! width.isVoid())
        item.width = width;

    auto height = getProperty (IDs::height, node);
    if (! height.isVoid())
        item.height = height;

    auto grow = getProperty (IDs::flexGrow, node);
    if (! grow.isVoid())
        item.flexGrow = grow;

    auto flexShrink = getProperty (IDs::flexShrink, node);
    if (! flexShrink.isVoid())
        item.flexShrink = flexShrink;

    auto flexOrder = getProperty (IDs::flexOrder, node);
    if (! flexOrder.isVoid())
        item.order = flexOrder;

    auto alignSelf = getProperty (IDs::flexAlignSelf, node).toString();
    if (alignSelf == IDs::flexStart)
        item.alignSelf = juce::FlexItem::AlignSelf::flexStart;
    else if (alignSelf == IDs::flexEnd)
        item.alignSelf = juce::FlexItem::AlignSelf::flexEnd;
    else if (alignSelf == IDs::flexCenter)
        item.alignSelf = juce::FlexItem::AlignSelf::center;
    else if (alignSelf == IDs::flexAuto)
        item.alignSelf = juce::FlexItem::AlignSelf::autoAlign;
    else
        item.alignSelf = juce::FlexItem::AlignSelf::stretch;
}

juce::ValueTree Stylesheet::createDefaultStyle()
{
    juce::ValueTree style (IDs::style, {{ IDs::name, "default" }},
    {
        { IDs::nodes, {} },
        { IDs::classes, {}, {
            { "group", {{ IDs::border, 2 }, { IDs::flexDirection, IDs::flexDirColumn }} }
        } },
        { IDs::types, {}, {
            { "Slider", {{ IDs::border, 0 }} },
            { "ToggleButton", {{ IDs::border, 0 }, { IDs::maxHeight, 50 }} },
            { "TextButton", {{ IDs::border, 0 }, { IDs::maxHeight, 50 }} },
            { "ComboBox", {{ IDs::border, 0 }, { IDs::maxHeight, 50 }} }
        } }
    });

    return style;
}


} // namespace foleys
