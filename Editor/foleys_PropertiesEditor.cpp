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

int PropertiesEditor::PropertiesListModel::getNumRows()
{
    if (styleItem.isValid())
        return styleItem.getNumProperties();

    return 0;
}

void PropertiesEditor::PropertiesListModel::paintListBoxItem (int rowNumber,
                                                              juce::Graphics& g,
                                                              int width, int height,
                                                              bool rowIsSelected)
{
    juce::Graphics::ScopedSaveState state (g);

    auto bounds = juce::Rectangle<int> (0, 0, width, height);
    g.setColour (juce::Colours::silver);
    g.drawHorizontalLine (0, 0, width);
    g.drawHorizontalLine (height - 1, 0, width);

    g.setColour (juce::Colours::white);
    bounds.reduce (3, 1);

    if (styleItem.isValid() == false)
    {
        g.setFont (g.getCurrentFont().withStyle (juce::Font::italic));
        g.drawFittedText (TRANS("not set"), bounds, juce::Justification::left, 1);
        return;
    }

    const auto name = styleItem.getPropertyName (rowNumber);
    const auto value = styleItem.getProperty (name).toString();

    g.drawFittedText (name + ": " + value, bounds, juce::Justification::left, 1);
}


} // namespace foleys
