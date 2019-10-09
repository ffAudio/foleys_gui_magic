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
    addAndMakeVisible (propertiesList);

    nodeSelect.onChange = [&]()
    {
        if (style.isValid() == false)
            return;

        auto index = nodeSelect.getSelectedId();
        if (index >= 3000)
        {
            auto node = style.getChildWithName (IDs::classes).getChild (index - 3000);
            styleItem = node;
        }
        else if (index >= 2000)
        {
            auto node = style.getChildWithName (IDs::nodes).getChild (index - 2000);
            styleItem = node;
        }
        else if (index >= 1000)
        {
            auto node = style.getChildWithName (IDs::types).getChild (index - 1000);
            styleItem = node;
        }

        propertiesList.updateContent();
    };

    propertySelect.setEditableText (true);

    addAndMakeVisible (propertySelect);
    addAndMakeVisible (propertyAdd);

    propertyAdd.onClick = [&]
    {
        const auto name = propertySelect.getText();
        if (name.isEmpty())
            return;

        auto oldValue = styleItem.getProperty (name, {});
        styleItem.setProperty (name, oldValue, &undo);
        propertiesList.updateContent();
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
    propertiesList.updateContent();
    updatePopupMenu();

    nodeSelect.setText (TRANS ("Editing node"));
}

juce::ValueTree& PropertiesEditor::getNodeToEdit()
{
    return styleItem;
}

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

    propertySelect.clear();
    propertySelect.addItemList (builder.getAllLayoutPropertyNames(), 1);
    propertySelect.addItem (IDs::source.toString(), 900);
    propertySelect.addItem (IDs::parameter.toString(), 901);

    auto colourNames = builder.getAllColourNames();
    colourNames.sort (true);
    propertySelect.addItemList (colourNames, 1000);
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

    auto addPanel = bounds.removeFromBottom (buttonHeight);
    propertyAdd.setBounds (addPanel.removeFromRight (buttonHeight));
    propertySelect.setBounds (addPanel);

    propertiesList.setBounds (bounds.reduced (0, 2));
}

MagicBuilder& PropertiesEditor::getMagicBuilder()
{
    return builder;
}

void PropertiesEditor::valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                                 const juce::Identifier& property)
{
    propertiesList.updateContent();
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


//==============================================================================

PropertiesEditor::PropertiesListModel::PropertiesListModel (PropertiesEditor& editor)
  : propertiesEditor (editor),
    undo (propertiesEditor.undo)
{
}

int PropertiesEditor::PropertiesListModel::getNumRows()
{
    auto styleItem = propertiesEditor.getNodeToEdit();

    if (styleItem.isValid())
        return styleItem.getNumProperties();

    return 0;
}

juce::Component* PropertiesEditor::PropertiesListModel::refreshComponentForRow (int rowNumber,
                                                                                bool isRowSelected,
                                                                                juce::Component *existingComponentToUpdate)
{
    auto* component = dynamic_cast<PropertiesItem*>(existingComponentToUpdate);
    if (existingComponentToUpdate != nullptr && component == nullptr)
        delete existingComponentToUpdate;

    if (component == nullptr)
        component = new PropertiesItem (propertiesEditor);

    auto styleItem = propertiesEditor.getNodeToEdit();
    if (styleItem.isValid() && rowNumber < styleItem.getNumProperties())
    {
        const auto name = styleItem.getPropertyName (rowNumber);
        component->setProperty (name.toString(), styleItem.getPropertyAsValue (name, &undo, true));
    }
    else
    {
        component->setProperty ("undefined", juce::Value());
    }

    return component;
}

//==============================================================================

PropertiesEditor::PropertiesItem::PropertiesItem (PropertiesEditor& editor)
  : propertiesEditor (editor)
{
    value.setEditable (true);
    addAndMakeVisible (value);
    addChildComponent (valueSelect);
    addAndMakeVisible (remove);

    valueSelect.onChange = [&]
    {
        auto styleItem = propertiesEditor.getNodeToEdit();
        if (styleItem.isValid())
            styleItem.setProperty (name, valueSelect.getText(), &propertiesEditor.undo);
    };

    remove.onClick =[&]
    {
        auto styleItem = propertiesEditor.getNodeToEdit();
        if (styleItem.isValid())
            styleItem.removeProperty (name, &propertiesEditor.undo);
    };
}

void PropertiesEditor::PropertiesItem::setProperty (const juce::String& nameToDisplay, const juce::Value& propertyValue)
{
    auto& builder = propertiesEditor.getMagicBuilder();
    name = nameToDisplay;

    juce::StringArray choices;
    valueSelect.clear();

    if (name == IDs::source.toString())
        choices = builder.getPlotSourcesNames();
    else if (name == IDs::parameter.toString())
        choices = builder.getParameterNames();

    if (choices.isEmpty())
    {
        value.setVisible (true);
        valueSelect.setVisible (false);
        value.getTextValue().referTo (propertyValue);
    }
    else
    {
        auto index = choices.indexOf (propertyValue.getValue().toString());
        value.setVisible (false);
        valueSelect.setVisible (true);
        valueSelect.addItemList (choices, 1);
        if (index>= 0)
            valueSelect.setSelectedId (index + 1);
    }

    repaint();
}

void PropertiesEditor::PropertiesItem::paint (juce::Graphics& g)
{
    g.setColour (EditorColours::outline);
    g.drawHorizontalLine (getHeight()-1, 0, getWidth());
    g.setColour (EditorColours::text);
    g.drawFittedText (name, getLocalBounds().withWidth (getWidth() / 2), juce::Justification::left, 1);
}

void PropertiesEditor::PropertiesItem::resized()
{
    auto bounds = getLocalBounds().withLeft (getWidth() / 2);
    remove.setBounds (bounds.removeFromRight (getHeight()));
    value.setBounds (bounds);
    valueSelect.setBounds (bounds);
}


} // namespace foleys
