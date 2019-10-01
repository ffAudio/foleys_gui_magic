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


PropertiesEditor::PropertiesEditor()
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
            propertiesModel.setNodeToEdit (node);
        }
        else if (index >= 2000)
        {
            auto node = style.getChildWithName (IDs::nodes).getChild (index - 2000);
            propertiesModel.setNodeToEdit (node);
        }
        else if (index >= 1000)
        {
            auto node = style.getChildWithName (IDs::types).getChild (index - 1000);
            propertiesModel.setNodeToEdit (node);
        }

        propertiesList.updateContent();
    };
}

void PropertiesEditor::setStyle (juce::ValueTree styleToEdit)
{
    style = styleToEdit;
    updatePopupMenu();
}

void PropertiesEditor::setColourNames (juce::StringArray names)
{
    propertiesModel.setColourNames (names);
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
}

void PropertiesEditor::resized()
{
    auto bounds = getLocalBounds();
    nodeSelect.setBounds (bounds.removeFromTop (24));
    propertiesList.setBounds (bounds);
}

void PropertiesEditor::PropertiesListModel::setNodeToEdit (juce::ValueTree node)
{
    styleItem = node;
}

void PropertiesEditor::PropertiesListModel::setColourNames (juce::StringArray names)
{
    colourNames = names;
    colourNames.sort (true);
}

int PropertiesEditor::PropertiesListModel::getNumRows()
{
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
        component = new PropertiesItem();

    if (styleItem.isValid() && rowNumber < styleItem.getNumProperties())
    {
        const auto name = styleItem.getPropertyName (rowNumber);
        component->setProperty (name.toString(), styleItem.getPropertyAsValue (name, nullptr));
    }
    else
    {
        component->setProperty ("undefined", juce::Value());
    }

    return component;
}


PropertiesEditor::PropertiesItem::PropertiesItem()
{
    value.setEditable (true);
    addAndMakeVisible (value);
}

void PropertiesEditor::PropertiesItem::setProperty (const juce::String& nameToDisplay, const juce::Value& propertyValue)
{
    name = nameToDisplay;
    value.getTextValue().referTo (propertyValue);
}

void PropertiesEditor::PropertiesItem::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::white);
    g.drawFittedText (name, getLocalBounds().withWidth (getWidth() / 2), juce::Justification::left, 1);
}

void PropertiesEditor::PropertiesItem::resized()
{
    value.setBounds (getLocalBounds().withLeft (getWidth() / 2));
}


} // namespace foleys
