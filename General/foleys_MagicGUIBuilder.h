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
 The MagicBuilder is responsible to recreate the GUI from a single ValueTree.
 You can add your own factories to the builder to allow additional components.
 There is a template available that allows for instance a specific AudioProcessor
 to be supplied to components, that need it.
 */
class MagicBuilder : private juce::ValueTree::Listener
{
public:
    MagicBuilder (juce::Component& parent);

    virtual ~MagicBuilder();

    void restoreGUI (const juce::ValueTree& gui);

    /**
     Grant access to the stylesheet to look up visual and layout properties
     */
    Stylesheet& getStylesheet();

    /**
     Grants access to the main XML, that holds all information.
     */
    juce::ValueTree& getGuiTree();

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
     For each factory you can register a translation table, which will forward the colours from the
     Stylesheet to the Components.
     */
    void setColourTranslation (juce::Identifier type, std::vector<std::pair<juce::String, int>> mapping);

    /**
     Returns all possible properties (except colour names) for the editor.
     */
    juce::StringArray getAllLayoutPropertyNames() const;

    /**
     This method collects all names of colours, so the style editor can show a control to edit the colours
     */
    juce::StringArray getAllColourNames() const;

    /**
     This will create a default GUI, in case of AudioProcessors from AudioProcessor::getParameterTree().

     @param keepExisting if set to true, it will not change an existing root div tree,
                         if set to false, it will replace any existing data.
     */
    virtual void createDefaultGUITree (bool keepExisting) = 0;

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    /**
     This method sets the GUI in edit mode, that allows to drag the components around.
     */
    void setEditMode (bool shouldEdit);
    bool isEditModeOn() const;

    void setSelectedNode (const juce::ValueTree& node);
    const juce::ValueTree& getSelectedNode() const;
#endif

protected:

    virtual std::unique_ptr<Decorator> restoreNode (juce::Component& component, const juce::ValueTree& node) = 0;

    juce::UndoManager undo;
    juce::ValueTree   config;
    Stylesheet        stylesheet;

    std::map<juce::Identifier, std::vector<std::pair<juce::String, int>>> colourTranslations;

private:

    void valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                   const juce::Identifier& property) override;

    void valueTreeChildAdded (juce::ValueTree& parentTree,
                              juce::ValueTree& childWhichHasBeenAdded) override;

    void valueTreeChildRemoved (juce::ValueTree& parentTree,
                                juce::ValueTree& childWhichHasBeenRemoved,
                                int indexFromWhichChildWasRemoved) override;

    void valueTreeChildOrderChanged (juce::ValueTree& parentTreeWhoseChildrenHaveMoved,
                                     int oldIndex, int newIndex) override;

    void valueTreeParentChanged (juce::ValueTree& treeWhoseParentHasChanged) override;


    juce::Component& parent;

    std::unique_ptr<Decorator> root;

    bool editMode = false;
    juce::ValueTree selectedNode;

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
    MagicGUIBuilder (juce::Component& parent, AppType& app, MagicProcessorState* magicState);

    /**
     This method creates a default DOM from the MagicProcessorState. It will read the
     parameterTree() from the AudioProcessor. It does nothing, if magicState is not provided
     (e.g. if the project is not an audio plugin).
     */
    void createDefaultGUITree (bool keepExisting);

    void createDefaultFromParameters (juce::ValueTree& node, const juce::AudioProcessorParameterGroup& tree);

    void registerFactory (juce::Identifier type, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&, AppType&)> factory);

    void registerJUCEFactories();

protected:

    std::unique_ptr<Decorator> restoreNode (juce::Component& component, const juce::ValueTree& node);

private:

    AppType& app;

    MagicProcessorState* magicState;

    std::map<juce::Identifier, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&, AppType&)>> factories;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicGUIBuilder)
};

} // namespace foleys
