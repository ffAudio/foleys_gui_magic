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


void Stylesheet::setStyle (const juce::ValueTree& node)
{
    currentStyle = node;
}

juce::var Stylesheet::getProperty (const juce::Identifier& name, const juce::ValueTree& node, bool inherit, juce::ValueTree* definedHere) const
{
    if (inherit && node.hasProperty (name))
    {
        if (definedHere)
            *definedHere = node;

        return node.getProperty (name);
    }

    if (inherit && node.hasProperty (IDs::id))
    {
        auto styleNode = currentStyle.getChildWithName (IDs::nodes);
        auto idNode = styleNode.getChildWithName (node.getProperty (IDs::id).toString());
        if (idNode.hasProperty (name))
        {
            if (definedHere)
                *definedHere = idNode;

            return idNode.getProperty (name);
        }
    }

    auto classNames = node.getProperty (IDs::styleClass, {}).toString();
    for (auto className : juce::StringArray::fromTokens (classNames, " ", {}))
    {
        auto classesNode = currentStyle.getChildWithName (IDs::classes);
        auto classNode = classesNode.getChildWithName (className);
        if (classNode.hasProperty (name))
        {
            if (definedHere)
                *definedHere = classNode;

            return classNode.getProperty (name);
        }
    }

    if (inherit)
    {
        auto typeNode = currentStyle.getChildWithName (IDs::types).getChildWithName (node.getType());
        if (typeNode.isValid() && typeNode.hasProperty (name))
        {
            if (definedHere)
                *definedHere = typeNode;

            return typeNode.getProperty (name);
        }
    }

    auto parent = node.getParent();
    if (parent.isValid() && parent.getType() != IDs::magic)
        return getProperty (name, parent, false, definedHere);

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

juce::StringArray Stylesheet::getLookAndFeelNames() const
{
    juce::StringArray names;
    for (const auto& it : lookAndFeels)
        names.add (it.first);

    return names;
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

juce::ValueTree Stylesheet::getCurrentStyle() const
{
    return currentStyle;
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

bool Stylesheet::isClassNode (const juce::ValueTree& node) const
{
    auto testParent = currentStyle.getChildWithName (IDs::classes);
    if (testParent.isValid())
        return node.isAChildOf (testParent);

    return false;
}

bool Stylesheet::isTypeNode (const juce::ValueTree& node) const
{
    auto testParent = currentStyle.getChildWithName (IDs::types);
    if (testParent.isValid())
        return node.isAChildOf (testParent);

    return false;
}

bool Stylesheet::isIdNode (const juce::ValueTree& node) const
{
    auto testParent = currentStyle.getChildWithName (IDs::nodes);
    if (testParent.isValid())
        return node.isAChildOf (testParent);

    return false;
}

juce::ValueTree Stylesheet::createDefaultStyle()
{
    juce::ValueTree style (IDs::style, {{ IDs::name, "default" }},
    {
        { IDs::nodes, {} },
        { IDs::classes, {}, {
            { "plot-view", {
                { IDs::border, 2 },
                { IDs::backgroundColour, "black" },
                { IDs::borderColour, "silver" },
                { IDs::display, IDs::contents }
            } },
            { "nomargin", {
                { IDs::margin, 0 },
                { IDs::padding, 0 },
                { IDs::border, 0 }} },
            { "group", {
                { IDs::margin, 5 },
                { IDs::padding, 5 },
                { IDs::border, 2 },
                { IDs::flexDirection, IDs::flexDirColumn }} }
        } },
        { IDs::types, {}, {
            { "Slider", {{ IDs::border, 0 }, { "slider-textbox", "textbox-below" }} },
            { "ToggleButton", {{ IDs::border, 0 }, { IDs::maxHeight, 50 }, { IDs::captionSize, 0 }, { "text", "Active" }} },
            { "TextButton", {{ IDs::border, 0 }, { IDs::maxHeight, 50 }, { IDs::captionSize, 0 }} },
            { "ComboBox", {{ IDs::border, 0 }, { IDs::maxHeight, 50 }, { IDs::captionSize, 0 }} },
            { "Plot", {{ IDs::border, 0 }, { IDs::margin, 0 }, { IDs::padding, 0 }, { IDs::backgroundColour, "00000000" }} },
            { "XYDragComponent", {{ IDs::border, 0 }, { IDs::margin, 0 }, { IDs::padding, 0 }, { IDs::backgroundColour, "00000000" }} }
        } }
    });

    return style;
}

juce::StringArray Stylesheet::getAllClassesNames() const
{
    juce::StringArray names;
    auto classesNode = currentStyle.getChildWithName (IDs::classes);
    if (classesNode.isValid())
        for (const auto& child : classesNode)
            names.add (child.getType().toString());

    return names;
}


} // namespace foleys
