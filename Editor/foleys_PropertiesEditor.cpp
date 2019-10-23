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
    const auto openness = properties.getOpennessState();

    styleItem = node;

    const auto& stylesheet = builder.getStylesheet();

    properties.clear();

    if (styleItem.isValid() == false)
    {
        nodeSelect.setText (TRANS ("Nothing selected"));
        return;
    }

    addNodeProperties();

    addDecoratorProperties();

    juce::Array<juce::PropertyComponent*> additional;

    if (stylesheet.isClassNode (styleItem))
    {
        for (auto factoryName : builder.getFactoryNames())
            addTypeProperties (factoryName, {});
    }
    else
    {
        addTypeProperties (styleItem.getType(), additional);
    }

    if (styleItem.getType() == IDs::view || stylesheet.isClassNode (styleItem))
        addContainerProperties();

    addFlexItemProperties();

    if (stylesheet.isClassNode (styleItem))
        nodeSelect.setText (TRANS ("Class: ") + styleItem.getType().toString(), juce::dontSendNotification);
    else if (stylesheet.isTypeNode (styleItem))
        nodeSelect.setText (TRANS ("Type: ") + styleItem.getType().toString(), juce::dontSendNotification);
    else if (stylesheet.isIdNode (styleItem))
        nodeSelect.setText (TRANS ("Node: ") + styleItem.getType().toString(), juce::dontSendNotification);
    else
        nodeSelect.setText (TRANS ("Editing node"), juce::dontSendNotification);

    properties.restoreOpennessState (*openness);
}

juce::ValueTree& PropertiesEditor::getNodeToEdit()
{
    return styleItem;
}

//==============================================================================

void PropertiesEditor::createNewClass()
{
    static juce::String editorID { "styleClass" };

    juce::AlertWindow dlg (TRANS ("New style class"), TRANS ("Enter a name:"), juce::AlertWindow::QuestionIcon, this);
    dlg.addTextEditor (editorID, "class");
    dlg.addButton (TRANS ("Cancel"), 0);
    dlg.addButton (TRANS ("Ok"), 1);
    if (dlg.runModalLoop() == 0)
        return;

    if (auto* editor = dlg.getTextEditor (editorID))
    {
        auto name = editor->getText().replaceCharacters (".&$@ ", "---__");
        builder.getStylesheet().addNewStyleClass (name, &undo);
    }
}

//==============================================================================

void PropertiesEditor::addNodeProperties()
{
    juce::Array<juce::PropertyComponent*> array;

    array.add (new juce::TextPropertyComponent (styleItem.getPropertyAsValue (IDs::id, &undo, true), IDs::id.toString(), 64, false, true));

    auto classNames = builder.getStylesheet().getAllClassesNames();
    array.add (new juce::TextPropertyComponent (styleItem.getPropertyAsValue (IDs::styleClass, &undo, true), IDs::styleClass.toString(), 256, false, true));
    // FIXME add class choice
    array.add (new StyleTextPropertyComponent (builder, IDs::styleClass, styleItem));

    properties.addSection ("Node", array, false);
}

void PropertiesEditor::addDecoratorProperties()
{
    juce::Array<juce::PropertyComponent*> array;
    array.add (new StyleTextPropertyComponent (builder, IDs::caption, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::captionSize, styleItem));
    array.add (new StyleColourPropertyComponent (builder, IDs::captionColour, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::margin, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::padding, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::border, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::radius, styleItem));
    array.add (new StyleColourPropertyComponent (builder, IDs::borderColour, styleItem));
    array.add (new StyleColourPropertyComponent (builder, IDs::backgroundColour, styleItem));
    array.add (new StyleChoicePropertyComponent (builder, IDs::lookAndFeel, styleItem, builder.getStylesheet().getLookAndFeelNames()));
    array.add (new StyleTextPropertyComponent (builder, IDs::backgroundImage, styleItem));

    properties.addSection ("Decorator", array, false);
}

void PropertiesEditor::addTypeProperties (juce::Identifier type, juce::Array<juce::PropertyComponent*> additional)
{
    juce::Array<juce::PropertyComponent*> array;

    array.addArray (additional);

    for (const auto& p : builder.getSettableProperties (type))
    {
        if (auto* component = StylePropertyComponent::createComponent (builder, *p, styleItem))
            array.add (component);
    }

    for (auto colour : builder.getColourNames (type))
    {
        array.add (new StyleColourPropertyComponent (builder, colour, styleItem));
    }

    properties.addSection (type.toString(), array, false);
}

void PropertiesEditor::addFlexItemProperties()
{
    juce::Array<juce::PropertyComponent*> array;

    array.add (new StyleTextPropertyComponent (builder, IDs::width, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::height, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::minWidth, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::minHeight, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::maxWidth, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::maxHeight, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::flexGrow, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::flexShrink, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::flexOrder, styleItem));
    array.add (new StyleChoicePropertyComponent (builder, IDs::flexAlignSelf, styleItem, { IDs::flexStretch, IDs::flexStart, IDs::flexEnd, IDs::flexCenter, IDs::flexAuto }));

    properties.addSection ("Flex-Item", array, false);
}

void PropertiesEditor::addContainerProperties()
{
    juce::Array<juce::PropertyComponent*> array;

    array.add (new StyleChoicePropertyComponent (builder, IDs::display, styleItem, { IDs::contents, IDs::flexbox }));
    array.add (new StyleTextPropertyComponent (builder, IDs::throttle, styleItem));

    array.add (new StyleChoicePropertyComponent (builder, IDs::flexDirection, styleItem, { IDs::flexDirRow, IDs::flexDirRowReverse, IDs::flexDirColumn, IDs::flexDirColumnReverse }));
    array.add (new StyleChoicePropertyComponent (builder, IDs::flexWrap, styleItem, { IDs::flexNoWrap, IDs::flexWrapNormal, IDs::flexWrapReverse }));
    array.add (new StyleChoicePropertyComponent (builder, IDs::flexAlignContent, styleItem, { IDs::flexStretch, IDs::flexStart, IDs::flexEnd, IDs::flexCenter, IDs::flexSpaceAround, IDs::flexSpaceBetween }));
    array.add (new StyleChoicePropertyComponent (builder, IDs::flexAlignItems, styleItem, { IDs::flexStretch, IDs::flexStart, IDs::flexEnd, IDs::flexCenter }));
    array.add (new StyleChoicePropertyComponent (builder, IDs::flexJustifyContent, styleItem, { IDs::flexStart, IDs::flexEnd, IDs::flexCenter, IDs::flexSpaceAround, IDs::flexSpaceBetween }));

    properties.addSection ("Container", array, false);
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

        menu.addSeparator();
        menu.addItem (juce::PopupMenu::Item ("New Class...")
                      .setAction ([p = juce::Component::SafePointer<PropertiesEditor>(this)]() mutable
        {
            if (p != nullptr)
                p->createNewClass();
        }));

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
