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


void Stylesheet::setStyle (const juce::ValueTree& node)
{
    currentStyle = node;
}

bool Stylesheet::setMediaSize (int width, int height)
{
    mediaWidth = width;
    mediaHeight = height;

    return (validMediaRanges.width.contains (width) &&
            validMediaRanges.height.contains (height));
}

void Stylesheet::updateValidRanges()
{
    validMediaRanges = Stylesheet::SizeRange();

    for (const auto& styleClass : styleClasses)
    {
        const auto range = styleClass.second->getValidSizeRange();

        if (mediaWidth < range.width.getStart())
            validMediaRanges.width.setEnd (std::min (validMediaRanges.width.getEnd(), range.width.getStart()));
        else if (mediaWidth < range.width.getEnd())
            validMediaRanges.width = validMediaRanges.width.getIntersectionWith (range.width);
        else
            validMediaRanges.width.setStart (std::max (validMediaRanges.width.getStart(), range.width.getEnd()));

        if (mediaHeight < range.height.getStart())
            validMediaRanges.height.setEnd (std::min (validMediaRanges.height.getEnd(), range.height.getStart()));
        else if (mediaHeight < range.height.getEnd())
            validMediaRanges.height = validMediaRanges.height.getIntersectionWith (range.height);
        else
            validMediaRanges.height.setStart (std::max (validMediaRanges.height.getStart(), range.height.getEnd()));
    }
}

void Stylesheet::updateStyleClasses (MagicGUIBuilder& builder)
{
    styleClasses.clear();

    for (const auto& styleNode : currentStyle.getChildWithName (IDs::classes))
    {
        auto styleClass = std::make_unique<StyleClass>(styleNode);
        if (styleNode.hasProperty (IDs::active))
        {
            if (auto* magicState = builder.getProcessorState())
            {
                auto activePropertyName = styleNode.getProperty (IDs::active);
                auto p = magicState->getPropertyAsValue (activePropertyName.toString());
                styleClass->setActiveProperty (p);
                styleClass->addChangeListener (&builder);
            }
        }

        styleClasses [styleNode.getType().toString()] = std::move (styleClass);
    }
}

juce::var Stylesheet::getStyleProperty (const juce::Identifier& name, const juce::ValueTree& node, bool inherit, juce::ValueTree* definedHere) const
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
        if (className.isEmpty())
            continue;

        const auto& sc = styleClasses.find (className);
        if (sc == styleClasses.end())
            continue;

        const auto& styleClass = sc->second;

        auto classesNode = currentStyle.getChildWithName (IDs::classes);
        auto classNode = classesNode.getChildWithName (className);

        if (!styleClass->isRecursive() && !inherit)
            continue;

        if (styleClass->isActive() &&
            styleClass->isValidForSize (mediaWidth, mediaHeight))
        {
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
    }

    auto parent = node.getParent();
    if (parent.isValid() && parent.getType() != IDs::magic)
        return getStyleProperty (name, parent, false, definedHere);

    if (definedHere)
        *definedHere = juce::ValueTree();

    return {};
}

juce::Colour Stylesheet::parseColour (const juce::String& name)
{
    return juce::Colours::findColourForName (name, juce::Colour::fromString (name.length() < 8 ? "ff" + name : name));
}

juce::LookAndFeel* Stylesheet::getLookAndFeel (const juce::ValueTree& node) const
{
    auto lnf = getStyleProperty (IDs::lookAndFeel, node).toString();
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
    auto name = getStyleProperty (IDs::backgroundImage, node);
    if (name.isVoid())
        return {};

    return Resources::getImage (name.toString());
}

juce::Array<juce::Colour> Stylesheet::getBackgroundGradient (const juce::ValueTree& node) const
{
    auto text = getStyleProperty (IDs::backgroundGradient, node).toString();

    if (text.startsWith (IDs::linearGradient))
    {
        auto parameters = getParameters (text);
        if (parameters.size() < 2)
            return {};

        juce::Array<juce::Colour> colours;
        for (int i=0; i < parameters.size(); ++i)
            colours.add (Stylesheet::parseColour (parameters [i].trim()));

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

juce::ValueTree Stylesheet::addNewStyleClass (const juce::String& name, juce::UndoManager* undo)
{
    if (currentStyle.isValid() == false)
        return {};

    auto classesNode = currentStyle.getOrCreateChildWithName (IDs::classes, undo);
    return classesNode.getOrCreateChildWithName (name, undo);
}

void Stylesheet::deleteStyleClass (const juce::String& name, juce::UndoManager* undo)
{
    if (currentStyle.isValid() == false)
        return;

    auto classesNode = currentStyle.getOrCreateChildWithName (IDs::classes, undo);
    auto child = classesNode.getChildWithName (name);
    if (child.isValid())
        classesNode.removeChild (child, undo);
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
            { "Plot", {{ IDs::border, 0 }, { IDs::margin, 0 }, { IDs::padding, 0 }, { IDs::backgroundColour, "00000000" }, {IDs::radius, 0}} },
            { "XYDragComponent", {{ IDs::border, 0 }, { IDs::margin, 0 }, { IDs::padding, 0 }, { IDs::backgroundColour, "00000000" }, {IDs::radius, 0}} }
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

//==============================================================================

Stylesheet::StyleClass::StyleClass (juce::ValueTree style)
  : styleNode (style)
{
    recursive = styleNode.getProperty (IDs::recursive, false);

    const auto media = styleNode.getChildWithName (IDs::media);
    if (media.isValid())
    {
        validRange.width.setStart (media.getProperty (IDs::minWidth, 0));
        validRange.width.setEnd (media.getProperty (IDs::maxWidth, std::numeric_limits<int>::max()));
        validRange.height.setStart (media.getProperty (IDs::minHeight, 0));
        validRange.height.setEnd (media.getProperty (IDs::maxHeight, std::numeric_limits<int>::max()));
    }
}

bool Stylesheet::StyleClass::isRecursive() const
{
    return recursive;
}

bool Stylesheet::StyleClass::isValidForSize (int width, int height) const
{
    return validRange.width.contains (width) && validRange.height.contains (height);
}

bool Stylesheet::StyleClass::isActive() const
{
    return activeFlag.getValue();
}
Stylesheet::SizeRange Stylesheet::StyleClass::getValidSizeRange() const
{
    return validRange;
}

void Stylesheet::StyleClass::setActiveProperty (juce::Value& source)
{
    activeFlag.referTo (source);
    activeFlag.addListener (this);
}

void Stylesheet::StyleClass::valueChanged (juce::Value&)
{
    sendChangeMessage();
}


} // namespace foleys
