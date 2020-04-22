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

#pragma once

namespace foleys
{


/**
 The MagicGUIBuilder is responsible to recreate the GUI from a single ValueTree.
 You can add your own factories to the builder to allow additional components.
 */
class MagicGUIBuilder : public juce::ChangeListener,
                        private juce::ValueTree::Listener
{
public:
    MagicGUIBuilder (MagicProcessorState* magicStateToUse);

    virtual ~MagicGUIBuilder();

    /**
     Allows to set the GUI definition when reloading
     */
    void setConfigTree (const juce::ValueTree& config);

    /**
     This triggers the rebuild of the GUI with setting the parent component
     */
    void restoreGUI (juce::Component& parent);

    /**
     Grant access to the stylesheet to look up visual and layout properties
     */
    Stylesheet& getStylesheet();

    /**
     Grants access to the main XML, that holds all information.
     */
    juce::ValueTree& getConfigTree();

    /**
     Grants access to the GUI root node.
     */
    juce::ValueTree getGuiRootNode();

    /**
     Will call updateStylesheet(), updateComponents(), updateProperties() and updateLayout()
     */
    void updateAll();

    /**
     This selects the stylesheet node and sets it to the Stylesheet.
     If no stylesheet is found, a default one is created.
     */
    void updateStylesheet();

    /**
     Recreates all components from the <div/> tree.
     If no div tree is found, createDefaultGUITree is called to give subclasses
     a chance to create a suitable default.
     */
    void updateComponents();

    /**
     Updates the colours and properties for all components
     */
    void updateProperties (Decorator& item);

    /**
     Recalculates the layout of all components
     */
    void updateLayout();

    /**
     Register a factory for Components to be available in the GUI editor. If you need a reference to the application, you can capture that in the factory lambda.
     */
    void registerFactory (juce::Identifier type, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&)> factory);

    /**
     With that method you can register your custom LookAndFeel class and apply it to different components.
     */
    void registerLookAndFeel (juce::String name, std::unique_ptr<juce::LookAndFeel> lookAndFeel);

    /**
     Registers automatically the JUCE LookAndFeel classes (V1..V4 at the time of writing)
     */
    void registerJUCELookAndFeels();

    /**
     Calling this method will register the bundled JUCE factories to give already access to
     many JUCE Components.
     */
    void registerJUCEFactories();

    /**
     For each factory you can register a translation table, which will forward the colours from the
     Stylesheet to the Components.
     */
    void setColourTranslation (juce::Identifier type, std::vector<std::pair<juce::String, int>> mapping);

    /**
     This method traverses the dom and checks each style, if that property was defined.

     @param name the name of the property.
     @param node is the node in the DOM. This is used for inheritance by traversing upwards.
     */
    juce::var getStyleProperty (const juce::Identifier& name, const juce::ValueTree& node) const;

    /**
     Looks up the ColourId for a given type
     */
    int findColourId (juce::Identifier type, juce::Identifier name);

    /**
     Looks up the ColourId in all types. Finds only the first hit, if the names are not unique.
     */
    int findColourId (juce::Identifier name);

    /**
     This will go through all nodes and delete the reference to a class
     */
    void removeStyleClassReferences (juce::ValueTree tree, const juce::String& name);

    /**
     This method returns the names of colours for a certain Component type
     */
    juce::StringArray getColourNames (juce::Identifier type) const;

    /**
     This method collects all names of colours, so the style editor can show a control to edit the colours
     */
    juce::StringArray getAllColourNames() const;

    /**
     This resets the GUI to show a single empty container
     */
    void clearGUI();

    /**
     This will create a default GUI, in case of AudioProcessors from AudioProcessor::getParameterTree().

     @param keepExisting if set to true, it will not change an existing root div tree,
                         if set to false, it will replace any existing data.
     */
    void createDefaultGUITree (bool keepExisting);

    /**
     This creates a hierarchical DOM according to the parameters defined in an AudioProcessor
     */
    void createDefaultFromParameters (juce::ValueTree& node, const juce::AudioProcessorParameterGroup& tree);

    /**
     This is used to display a dialog box. It is called by the GUI editor, but in future it might be reached
     using the configured GUI.
     */
    void showOverlayDialog (std::unique_ptr<juce::Component> dialog);

    /**
     Closes a possibly open overlay dialog
     */
    void closeOverlayDialog();

    /**
     returns the names of all registered factories
     */
    juce::StringArray getFactoryNames() const;

    void addSettableProperty (juce::Identifier type, std::unique_ptr<SettableProperty> property);
    const std::vector<std::unique_ptr<SettableProperty>>& getSettableProperties (juce::Identifier type) const;

    /**
     Return the list of options
     */
    void populateSettableOptionsMenu (juce::ComboBox& comboBox, SettableProperty::PropertyType type) const;

    static juce::NamedValueSet makeJustificationsChoices();

    void changeListenerCallback (juce::ChangeBroadcaster* sender) override;

    /**
     Lookup the default value of the property
     */
    juce::var getPropertyDefaultValue (juce::Identifier type, juce::Identifier property) const;

    MagicProcessorState* getProcessorState();

    juce::UndoManager& getUndoManager();

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    void attachToolboxToWindow (juce::Component& window);

    /**
     This method sets the GUI in edit mode, that allows to drag the components around.
     */
    void setEditMode (bool shouldEdit);
    bool isEditModeOn() const;

    void setSelectedNode (const juce::ValueTree& node);
    const juce::ValueTree& getSelectedNode() const;

    void draggedItemOnto (juce::ValueTree dropped, juce::ValueTree target, int index=-1);

    ToolBox& getMagicToolBox();
#endif

private:

    std::unique_ptr<Decorator> restoreNode (juce::Component& component, const juce::ValueTree& node);

    juce::UndoManager undo;
    juce::ValueTree   config;
    Stylesheet        stylesheet;

    std::map<juce::Identifier, std::vector<std::pair<juce::String, int>>> colourTranslations;

    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override;

    void valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&) override;

    void valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int) override;

    void valueTreeChildOrderChanged (juce::ValueTree&, int, int) override;

    void valueTreeParentChanged (juce::ValueTree&) override;

    //==============================================================================

    juce::Component::SafePointer<juce::Component> parent;

    MagicProcessorState* magicState;

    std::unique_ptr<Decorator> root;

    std::unique_ptr<juce::Component> overlayDialog;

    std::map<juce::Identifier, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&)>> factories;
    std::map<juce::Identifier, std::vector<std::unique_ptr<SettableProperty>>> settableProperties;
    const std::vector<std::unique_ptr<SettableProperty>> emptyPropertyList;

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    bool editMode = false;
    juce::ValueTree selectedNode;

    std::unique_ptr<ToolBox> magicToolBox;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicGUIBuilder)
};


} // namespace foleys
