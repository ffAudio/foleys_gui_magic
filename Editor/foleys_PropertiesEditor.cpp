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


PropertiesEditor::PropertiesEditor (MagicGUIBuilder& builderToEdit)
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
    updatePopupMenu();

    const auto& stylesheet = builder.getStylesheet();

    properties.clear();

    if (styleItem.isValid() == false)
    {
        nodeSelect.setText (TRANS ("Nothing selected"));
        return;
    }

    updatePopupMenu();

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

void PropertiesEditor::deleteClass (const juce::String& name)
{
    auto& stylesheet = builder.getStylesheet();
    stylesheet.deleteStyleClass (name, &undo);
    builder.removeStyleClassReferences (builder.getGuiRootNode(), name);
}

//==============================================================================

void PropertiesEditor::addNodeProperties()
{
    const auto& stylesheet = builder.getStylesheet();

    if (stylesheet.isTypeNode (styleItem) ||
        stylesheet.isIdNode (styleItem))
        return;

    juce::Array<juce::PropertyComponent*> array;

    if (stylesheet.isClassNode (styleItem))
    {
        array.add (new juce::BooleanPropertyComponent (styleItem.getPropertyAsValue (IDs::recursive, &undo), IDs::recursive.toString(), {}));
        array.add (new StyleChoicePropertyComponent (builder, IDs::active, styleItem, SettableProperty::Property));

        auto media = styleItem.getOrCreateChildWithName (IDs::media, &undo);
        array.add (new juce::TextPropertyComponent (media.getPropertyAsValue (IDs::minWidth, &undo), IDs::minWidth.toString(), 10, false));
        array.add (new juce::TextPropertyComponent (media.getPropertyAsValue (IDs::maxWidth, &undo), IDs::maxWidth.toString(), 10, false));
        array.add (new juce::TextPropertyComponent (media.getPropertyAsValue (IDs::minHeight, &undo), IDs::minHeight.toString(), 10, false));
        array.add (new juce::TextPropertyComponent (media.getPropertyAsValue (IDs::maxHeight, &undo), IDs::maxHeight.toString(), 10, false));
        properties.addSection ("Class", array, false);
        return;
    }

    array.add (new juce::TextPropertyComponent (styleItem.getPropertyAsValue (IDs::id, &undo, true), IDs::id.toString(), 64, false, true));

    if (styleItem == builder.getGuiRootNode())
    {
        array.add (new juce::BooleanPropertyComponent (styleItem.getPropertyAsValue (IDs::resizable, &undo), IDs::resizable.toString(), {}));
        array.add (new juce::BooleanPropertyComponent (styleItem.getPropertyAsValue (IDs::resizeCorner, &undo), IDs::resizeCorner.toString(), {}));
        array.add (new juce::TextPropertyComponent (styleItem.getPropertyAsValue (IDs::width, &undo), IDs::width.toString(), 8, false));
        array.add (new juce::TextPropertyComponent (styleItem.getPropertyAsValue (IDs::height, &undo), IDs::height.toString(), 8, false));
        array.add (new juce::TextPropertyComponent (styleItem.getPropertyAsValue (IDs::minWidth, &undo), IDs::minWidth.toString(), 8, false));
        array.add (new juce::TextPropertyComponent (styleItem.getPropertyAsValue (IDs::maxWidth, &undo), IDs::maxWidth.toString(), 8, false));
        array.add (new juce::TextPropertyComponent (styleItem.getPropertyAsValue (IDs::minHeight, &undo), IDs::minHeight.toString(), 8, false));
        array.add (new juce::TextPropertyComponent (styleItem.getPropertyAsValue (IDs::maxHeight, &undo), IDs::maxHeight.toString(), 8, false));
    }

    auto classNames = builder.getStylesheet().getAllClassesNames();
    array.add (new MultiListPropertyComponent (styleItem.getPropertyAsValue (IDs::styleClass, &undo, true), IDs::styleClass.toString(), classNames));
    properties.addSection ("Node", array, false);
}

void PropertiesEditor::addDecoratorProperties()
{
    juce::Array<juce::PropertyComponent*> array;
    array.add (new StyleChoicePropertyComponent (builder, IDs::visibility, styleItem, SettableProperty::Property));
    array.add (new StyleTextPropertyComponent (builder, IDs::caption, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::captionSize, styleItem));
    array.add (new StyleColourPropertyComponent (builder, IDs::captionColour, styleItem));
    array.add (new StyleChoicePropertyComponent (builder, IDs::captionPlacement, styleItem, SettableProperty::Justification));
    array.add (new StyleTextPropertyComponent (builder, IDs::tooltip, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::margin, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::padding, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::border, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::radius, styleItem));
    array.add (new StyleColourPropertyComponent (builder, IDs::borderColour, styleItem));
    array.add (new StyleColourPropertyComponent (builder, IDs::backgroundColour, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::tabCaption, styleItem));
    array.add (new StyleColourPropertyComponent (builder, IDs::tabColour, styleItem));
    array.add (new StyleChoicePropertyComponent (builder, IDs::lookAndFeel, styleItem, builder.getStylesheet().getLookAndFeelNames()));
    array.add (new StyleChoicePropertyComponent (builder, IDs::backgroundImage, styleItem, Resources::getResourceFileNames()));
    array.add (new StyleChoicePropertyComponent (builder, IDs::imagePlacement, styleItem, { IDs::imageCentred, IDs::imageFill, IDs::imageStretch }));
    array.add (new StyleTextPropertyComponent (builder, IDs::backgroundAlpha, styleItem));
    array.add (new StyleTextPropertyComponent (builder, IDs::backgroundGradient, styleItem));

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

    array.add (new StyleChoicePropertyComponent (builder, IDs::display, styleItem, { IDs::contents, IDs::flexbox, IDs::tabbed }));
    array.add (new StyleTextPropertyComponent (builder, IDs::repaintHz, styleItem));

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
    auto* popup = nodeSelect.getRootMenu();
    popup->clear();

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

        if (builder.getStylesheet().isClassNode (styleItem))
        {
            auto name = styleItem.getType().toString();
            menu.addItem (juce::PopupMenu::Item ("Delete Class \"" + name + "\"")
                          .setAction ([p = juce::Component::SafePointer<PropertiesEditor>(this), name]() mutable
            {
                if (p != nullptr)
                    p->deleteClass (name);
            }));
        }

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

MagicGUIBuilder& PropertiesEditor::getMagicBuilder()
{
    return builder;
}

void PropertiesEditor::valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&)
{
    updatePopupMenu();
}

void PropertiesEditor::valueTreeChildRemoved (juce::ValueTree&,
                                              juce::ValueTree& childWhichHasBeenRemoved,
                                              int)
{
    if (childWhichHasBeenRemoved == styleItem)
        setNodeToEdit ({});
}


} // namespace foleys
