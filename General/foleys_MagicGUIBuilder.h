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
 A SettableProperty is a value that can be selected by the designer and will be
 set for the Component each time the ValueTree is loaded.
 */
struct SettableProperty
{
    using OptionsMap=std::map<juce::String, juce::var>;
    juce::Identifier                                                    name;
    OptionsMap                                                          options;
    std::function<void(juce::Component*, juce::var, const OptionsMap&)> setter;
    juce::String                                                        defaultValue;
};

/**
 The MagicBuilder is responsible to recreate the GUI from a single ValueTree.
 You can add your own factories to the builder to allow additional components.
 There is a template available that allows for instance a specific AudioProcessor
 to be supplied to components, that need it.
 */
class MagicBuilder : private juce::ValueTree::Listener
{
public:
    MagicBuilder();

    virtual ~MagicBuilder();

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
     Recalculates the layout of all components
     */
    void updateLayout();

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
    virtual void registerJUCEFactories() {}

    /**
     For each factory you can register a translation table, which will forward the colours from the
     Stylesheet to the Components.
     */
    void setColourTranslation (juce::Identifier type, std::vector<std::pair<juce::String, int>> mapping);

    /**
     Looks up the ColourId for a given type
     */
    int findColourId (juce::Identifier type, juce::Identifier name);

    /**
     Looks up the ColourId in all types. Finds only the first hit, if the names are not unique.
     */
    int findColourId (juce::Identifier name);

    /**
     Returns all possible properties (except colour names) for the editor.
     */
    juce::StringArray getAllLayoutPropertyNames() const;

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
    virtual void createDefaultGUITree (bool keepExisting) = 0;

    /**
     This creates a hierarchical DOM according to the parameters defined in an AudioProcessor
     */
    void createDefaultFromParameters (juce::ValueTree& node, const juce::AudioProcessorParameterGroup& tree);

    /**
     returns the names of all registered factories
     */
    virtual juce::StringArray getFactoryNames() const = 0;

    /**
     Returns the IDs of AudioProcessorParameters for selection
     */
    virtual juce::StringArray getParameterNames() const = 0;

    /**
     Returns the IDs of MagicPlotSources for selection
     */
    virtual juce::StringArray getPlotSourcesNames() const = 0;

    void addSettableProperty (juce::Identifier type, SettableProperty property);
    std::vector<SettableProperty> getSettableProperties (juce::Identifier type) const;


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
    juce::UndoManager& getUndoManager();
#endif

protected:

    virtual std::unique_ptr<Decorator> restoreNode (juce::Component& component, const juce::ValueTree& node) = 0;

    juce::UndoManager undo;
    juce::ValueTree   config;
    Stylesheet        stylesheet;

    std::map<juce::Identifier, std::vector<std::pair<juce::String, int>>> colourTranslations;

private:

    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override
    { updateAll(); }

    void valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&) override
    { updateAll(); }

    void valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int) override
    { updateAll(); }

    void valueTreeChildOrderChanged (juce::ValueTree&, int, int) override
    { updateAll(); }

    void valueTreeParentChanged (juce::ValueTree&) override
    { updateAll(); }

    //==============================================================================

    juce::Component::SafePointer<juce::Component> parent;

    std::unique_ptr<Decorator> root;

    bool editMode = false;
    juce::ValueTree selectedNode;

    std::map<juce::Identifier, std::vector<SettableProperty>> settableProperties;

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    std::unique_ptr<ToolBox> magicToolBox;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicBuilder)
};

/**
 The MagicGUIBuilder is responsible to recreate the GUI from a single ValueTree.
 You can add your own factories to the builder to allow additional components.
 The template parameter AppType specifies a type, that each components gets a reference of,
 it is intended to be eithe AudioProcessor, JUCEApplication or your own MainWindow, it's up to you.
 */
template <class AppType>
class MagicGUIBuilder : public MagicBuilder
{
public:

    /**
     Creates the MagicGUIBuilder. This is responsible to create the GUI elements from the ValueTree.
     To allow custom Components to be created with a backlink to the containing application.
     This can be an AudioProcessor or it's descendents. Or it could be your custom MainComponent.
     In which case you have to template it with your own type. Now you can reference it in the
     factory method.

     @param parent      is the component, the GUI will be imbedded into.
     @param app         is the model the components might connect to.
     @param magicState  an optional pointer to the MagicProcessorState. This is only useful
                        for AudioProcessors, but will allow automatic connection of parameters.
     */
    MagicGUIBuilder (AppType& appToUse, MagicProcessorState* magicStateToUse);

    /**
     Calling this method will register the bundled JUCE factories to give already access to
     many JUCE Components.
     */
    void registerJUCEFactories() override;


    /**
     This method creates a default DOM from the MagicProcessorState. It will read the
     parameterTree() from the AudioProcessor. It does nothing, if magicState is not provided
     (e.g. if the project is not an audio plugin).
     */
    void createDefaultGUITree (bool keepExisting) override;


    void registerFactory (juce::Identifier type, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&, AppType&)> factory);

    /**
     returns the names of all registered factories
     */
    juce::StringArray getFactoryNames() const override;

    /**
     Returns the IDs of AudioProcessorParameters for selection
     */
    juce::StringArray getParameterNames() const override;

    /**
     Returns the IDs of MagicPlotSources for selection
     */
    juce::StringArray getPlotSourcesNames() const override;


protected:

    std::unique_ptr<Decorator> restoreNode (juce::Component& component, const juce::ValueTree& node) override;

private:

    AppType& app;

    MagicProcessorState* magicState;

    std::map<juce::Identifier, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&, AppType&)>> factories;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicGUIBuilder)
};

} // namespace foleys
