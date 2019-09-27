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

namespace foleys
{

namespace IDs
{
    static juce::Identifier nodes     { "Nodes"   };
    static juce::Identifier classes   { "Classes" };
    static juce::Identifier types     { "Types"   };

    static juce::Identifier flexDirection       { "flex-direction" };
    static juce::String     flexDirRow          { "row" };
    static juce::String     flexDirRowReverse   { "row-reverse" };
    static juce::String     flexDirColumn       { "column" };
    static juce::String     flexDirColumnReverse { "column-reverse" };

    static juce::Identifier flexGrow    { "flex-grow" };
    static juce::Identifier flexShrink  { "flex-shrink" };

    static juce::Identifier minWidth    { "min-width" };
    static juce::Identifier maxWidth    { "max-width" };
    static juce::Identifier minHeight   { "min-height" };
    static juce::Identifier maxHeight   { "max-height" };

}

void Stylesheet::setStyle (const juce::ValueTree& node)
{
    currentStyle = node;
}

juce::var Stylesheet::getProperty (const juce::Identifier& name, const juce::ValueTree& node) const
{
    if (node.hasProperty (IDs::id))
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

    auto typeNode = currentStyle.getChildWithName (IDs::types).getChildWithName (node.getType());
    if (typeNode.isValid() && typeNode.hasProperty (name))
        return typeNode.getProperty (name);

    auto parent = node.getParent();
    if (parent.isValid() && parent.getType() != IDs::magic)
        return getProperty (name, parent);

    return {};
}

juce::LookAndFeel* Stylesheet::getLookAndFeel (const juce::ValueTree& node)
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

    auto grow = getProperty (IDs::flexGrow, node);
    if (! grow.isVoid())
        item.flexGrow = grow;

    auto flexShrink = getProperty (IDs::flexShrink, node);
    if (! flexShrink.isVoid())
        item.flexShrink = flexShrink;

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
