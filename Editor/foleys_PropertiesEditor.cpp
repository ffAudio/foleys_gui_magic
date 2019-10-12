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


PropertiesEditor::PropertiesEditor (MagicBuilder& builderToEdit)
  : builder (builderToEdit),
    undo (builder.getUndoManager())
{
    addAndMakeVisible (nodeSelect);
    addAndMakeVisible (properties);

    nodeSelect.onChange = [&]()
    {
        if (style.isValid() == false)
            return;

        auto index = nodeSelect.getSelectedId();
        if (index >= 3000)
        {
            auto node = style.getChildWithName (IDs::classes).getChild (index - 3000);
            setNodeToEdit (node);
        }
        else if (index >= 2000)
        {
            auto node = style.getChildWithName (IDs::nodes).getChild (index - 2000);
            setNodeToEdit (node);
        }
        else if (index >= 1000)
        {
            auto node = style.getChildWithName (IDs::types).getChild (index - 1000);
            setNodeToEdit (node);
        }

    };
}

void PropertiesEditor::setStyle (juce::ValueTree styleToEdit)
{
    style = styleToEdit;
    updatePopupMenu();

    style.addListener (this);
}

void PropertiesEditor::setNodeToEdit (juce::ValueTree node)
{
    styleItem = node;

    properties.clear();

    if (styleItem.isValid() == false)
    {
        nodeSelect.setText (TRANS ("Nothing selected"));
        return;
    }

    addNodeProperties();

    addDecoratorProperties();

    if (builder.getStylesheet().isClassNode (styleItem))
    {
        for (auto factoryName : builder.getFactoryNames())
            addTypeProperties (factoryName, false);
    }
    else
    {
        addTypeProperties (styleItem.getType());
    }

    if (styleItem.getType() == IDs::view)
        addFlexContainerProperties();

    addFlexItemProperties();

    if (styleItem == builder.getGuiRootNode() || styleItem.isAChildOf (builder.getGuiRootNode()))
        nodeSelect.setText (TRANS ("Editing node"));
}

juce::ValueTree& PropertiesEditor::getNodeToEdit()
{
    return styleItem;
}

//==============================================================================

void PropertiesEditor::addNodeProperties (bool shouldBeOpen)
{
    juce::Array<juce::PropertyComponent*> array;

    array.add (new juce::TextPropertyComponent (styleItem.getPropertyAsValue (IDs::id, &undo, true), IDs::id.toString(), 64, false, true));

    auto classNames = builder.getStylesheet().getAllClassesNames();
    // FIXME add class choice

    properties.addSection ("Node", array, shouldBeOpen);
}

void PropertiesEditor::addDecoratorProperties (bool shouldBeOpen)
{
    juce::Array<juce::PropertyComponent*> array;

    properties.addSection ("Decorator", array, shouldBeOpen);
}

void PropertiesEditor::addTypeProperties (juce::Identifier type, bool shouldBeOpen)
{
    juce::Array<juce::PropertyComponent*> array;

    for (auto p : builder.getSettableProperties (type))
    {
        juce::StringArray       choices;
        juce::Array<juce::var>  values;

        for (auto o : p.options)
            choices.add (o.first);

        array.add (new StyleChoicePropertyComponent (builder, p.name, styleItem, choices));
    }

    for (auto colour : builder.getColourNames (type))
    {
        array.add (new StyleTextPropertyComponent (builder, colour, styleItem));
    }

    properties.addSection (type.toString(), array, shouldBeOpen);
}

void PropertiesEditor::addFlexItemProperties (bool shouldBeOpen)
{
    juce::Array<juce::PropertyComponent*> array;

    array.add (new StyleChoicePropertyComponent (builder, IDs::flexAlignSelf, styleItem, { IDs::flexStretch, IDs::flexStart, IDs::flexEnd, IDs::flexCenter, IDs::flexAuto }));

    properties.addSection ("Flex-Item", array, shouldBeOpen);
}

void PropertiesEditor::addFlexContainerProperties (bool shouldBeOpen)
{
    juce::Array<juce::PropertyComponent*> array;

    array.add (new StyleChoicePropertyComponent (builder, IDs::flexDirection, styleItem, { IDs::flexDirRow, IDs::flexDirRowReverse, IDs::flexDirColumn, IDs::flexDirColumnReverse }));
    array.add (new StyleChoicePropertyComponent (builder, IDs::flexWrap, styleItem, { IDs::flexNoWrap, IDs::flexWrapNormal, IDs::flexWrapReverse }));
    array.add (new StyleChoicePropertyComponent (builder, IDs::flexAlignContent, styleItem, { IDs::flexStretch, IDs::flexStart, IDs::flexEnd, IDs::flexCenter, IDs::flexSpaceAround, IDs::flexSpaceBetween }));
    array.add (new StyleChoicePropertyComponent (builder, IDs::flexAlignItems, styleItem, { IDs::flexStretch, IDs::flexStart, IDs::flexEnd, IDs::flexCenter }));
    array.add (new StyleChoicePropertyComponent (builder, IDs::flexJustifyContent, styleItem, { IDs::flexStart, IDs::flexEnd, IDs::flexCenter, IDs::flexSpaceAround, IDs::flexSpaceBetween }));

    properties.addSection ("Flex-Container", array, shouldBeOpen);
}

//==============================================================================

void PropertiesEditor::updatePopupMenu()
{
    nodeSelect.clear();
    auto* popup = nodeSelect.getRootMenu();

    auto typesNode = style.getChildWithName (IDs::types);
    if (typesNode.isValid())
    {
        juce::PopupMenu menu;
        int index = 1000;
        for (const auto& child : typesNode)
            menu.addItem (juce::PopupMenu::Item ("Type: " + child.getType().toString()).setID (index++));

        popup->addSubMenu ("Types", menu);
    }

    auto nodesNode = style.getChildWithName (IDs::nodes);
    if (nodesNode.isValid())
    {
        int index = 2000;
        juce::PopupMenu menu;
        for (const auto& child : nodesNode)
            menu.addItem (juce::PopupMenu::Item ("Node: " + child.getType().toString()).setID (index++));

        popup->addSubMenu ("Nodes", menu);
    }

    auto classesNode = style.getChildWithName (IDs::classes);
    if (classesNode.isValid())
    {
        int index = 3000;
        juce::PopupMenu menu;
        for (const auto& child : classesNode)
            menu.addItem (juce::PopupMenu::Item ("Class: " + child.getType().toString()).setID (index++));

        popup->addSubMenu ("Classes", menu);
    }

}

void PropertiesEditor::paint (juce::Graphics& g)
{
    g.setColour (EditorColours::outline);
    g.drawRect (getLocalBounds(), 1);
}

void PropertiesEditor::resized()
{
    const auto buttonHeight = 24;
    auto bounds = getLocalBounds().reduced (1);

    nodeSelect.setBounds (bounds.removeFromTop (buttonHeight));

    properties.setBounds (bounds.reduced (0, 2));
}

MagicBuilder& PropertiesEditor::getMagicBuilder()
{
    return builder;
}

void PropertiesEditor::valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                                 const juce::Identifier& property)
{
}

void PropertiesEditor::valueTreeChildAdded (juce::ValueTree& parentTree,
                                            juce::ValueTree& childWhichHasBeenAdded)
{
    updatePopupMenu();
}

void PropertiesEditor::valueTreeChildRemoved (juce::ValueTree& parentTree,
                                              juce::ValueTree& childWhichHasBeenRemoved,
                                              int indexFromWhichChildWasRemoved)
{
    if (childWhichHasBeenRemoved == styleItem)
        setNodeToEdit ({});
}


} // namespace foleys
