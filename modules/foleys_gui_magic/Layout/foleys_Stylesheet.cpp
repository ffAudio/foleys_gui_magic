/*
 ==============================================================================
    Copyright (c) 2019-2023 Foleys Finest Audio - Daniel Walz
    All rights reserved.

    **BSD 3-Clause License**

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

 ==============================================================================

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

#include "foleys_Stylesheet.h"

namespace foleys
{

Stylesheet::Stylesheet (MagicGUIBuilder& builderToUse) : builder (builderToUse)
{
    setColourPalette();

    currentStyle.addListener(this);
    currentPalette.addListener(this);
}

Stylesheet::~Stylesheet()
{
    currentStyle.removeListener(this);
    currentPalette.removeListener(this);
}

void Stylesheet::setStyle (const juce::ValueTree& node)
{
    currentStyle = node;
    setColourPalette();
}

bool Stylesheet::setMediaSize (int width, int height)
{
    mediaWidth = width;
    mediaHeight = height;

    return (validMediaRanges.width.contains (width) &&
            validMediaRanges.height.contains (height));
}

void Stylesheet::setColourPalette ()
{
    if (! currentStyle.isValid())
        return;

    auto* undo = &builder.getUndoManager();
    auto palettesNode = currentStyle.getOrCreateChildWithName (IDs::palettes, undo);

    if (palettesNode.getNumChildren() == 0)
        palettesNode.appendChild (juce::ValueTree ("default"), undo);

    currentPalette = palettesNode.getChild (0);
    currentPalette.addListener (this);
}

void Stylesheet::addPaletteEntry (const juce::String& name, juce::Colour colour, bool keepIfExists)
{
    if (! currentPalette.isValid())
        return;

    if (! keepIfExists || ! currentPalette.hasProperty (name))
        currentPalette.setProperty (name, colour.toString(), &builder.getUndoManager());
}

juce::StringArray Stylesheet::getPaletteEntryNames() const
{
    juce::StringArray names;

    if (currentPalette.isValid())
        for (int i = 0; i < currentPalette.getNumProperties(); ++i)
            names.add (currentPalette.getPropertyName (i).toString());

    return names;
}

juce::ValueTree Stylesheet::getCurrentPalette()
{
    return currentPalette;
}

void Stylesheet::valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier& name)
{
    if (name.toString().contains("color"))
        builder.updateColours();
    else
        builder.updateComponents();
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

void Stylesheet::updateStyleClasses()
{
    styleClasses.clear();

    for (const auto& styleNode : currentStyle.getChildWithName (IDs::classes))
    {
        auto styleClass = std::make_unique<StyleClass>(styleNode);
        if (styleNode.hasProperty (IDs::active))
        {
            auto activePropertyName = styleNode.getProperty (IDs::active);
            auto p = builder.getMagicState().getPropertyAsValue (activePropertyName.toString());
            styleClass->setActiveProperty (p);
            styleClass->addChangeListener (&builder);
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

    if (inherit && node.hasProperty (IDs::id) && node.getProperty (IDs::id).toString().isNotEmpty())
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

    return builder.getPropertyDefaultValue (name);
}

juce::Colour Stylesheet::getColour (const juce::String& name) const
{
    if (name.isEmpty())
        return juce::Colours::transparentBlack;

    if (name [0] == '$')
    {
        if (currentPalette.isValid())
        {
            auto value = currentPalette.getProperty (name.substring (1), "00000000").toString();
            return Stylesheet::parseColour (value);
        }

        return juce::Colours::transparentBlack;
    }

    return Stylesheet::parseColour (name);
}

juce::Colour Stylesheet::parseColour (const juce::String& name)
{
    if (name.startsWithIgnoreCase ("transparent"))
        return juce::Colours::transparentBlack;

    return juce::Colours::findColourForName (name, juce::Colour::fromString (name.length() < 8 ? "ff" + name : name));
}

juce::LookAndFeel* Stylesheet::getLookAndFeel (const juce::ValueTree& node) const
{
    auto lnfNode = getStyleProperty (IDs::lookAndFeel, node);
    if (lnfNode.isVoid())
        return nullptr;

    auto lnf = lnfNode.toString();
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

bool Stylesheet::isColourPaletteNode (const juce::ValueTree& node) const
{
    auto testParent = currentStyle.getChildWithName (IDs::palettes);
    if (testParent.isValid())
        return node.isAChildOf (testParent);

    return false;
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

void Stylesheet::addListener (juce::ValueTree::Listener* listener)
{
    currentStyle.addListener (listener);
}

void Stylesheet::removeListener (juce::ValueTree::Listener* listener)
{
    currentStyle.removeListener (listener);
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
