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

class MagicBuilder;

class PropertiesEditor  : public juce::Component,
                          private juce::ValueTree::Listener
{
public:
    PropertiesEditor (MagicBuilder& builder);

    void setStyle (juce::ValueTree style);

    void setNodeToEdit (juce::ValueTree node);

    void paint (juce::Graphics&) override;
    void resized() override;

private:

    void updatePopupMenu();

    class PropertiesListModel;

    class PropertiesItem : public juce::Component
    {
    public:
        PropertiesItem (PropertiesListModel& model);

        void setProperty (const juce::String& name, const juce::Value& propertyValue);
        void paint (juce::Graphics&) override;
        void resized() override;

    private:
        PropertiesListModel& propertiesModel;

        juce::String     name;
        juce::Label      value;
        juce::TextButton remove { "X" };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertiesItem)
    };

    class PropertiesListModel : public juce::ListBoxModel
    {
    public:
        PropertiesListModel (PropertiesEditor& editor);

        void setNodeToEdit (juce::ValueTree node);

        int getNumRows() override;

        juce::Component* refreshComponentForRow (int rowNumber,
                                                 bool isRowSelected,
                                                 juce::Component *existingComponentToUpdate) override;

        void paintListBoxItem (int rowNumber,
                               juce::Graphics& g,
                               int width, int height,
                               bool rowIsSelected) override {};

        juce::ValueTree&  getCurrentStyleItem();

        PropertiesEditor&   propertiesEditor;
        juce::UndoManager&  undo;

    private:
        juce::ValueTree     styleItem;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertiesListModel)
    };

    void valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                   const juce::Identifier& property) override;

    void valueTreeChildAdded (juce::ValueTree& parentTree,
                              juce::ValueTree& childWhichHasBeenAdded) override;

    void valueTreeChildRemoved (juce::ValueTree& parentTree,
                                juce::ValueTree& childWhichHasBeenRemoved,
                                int indexFromWhichChildWasRemoved) override;

    void valueTreeChildOrderChanged (juce::ValueTree& parentTreeWhoseChildrenHaveMoved,
                                     int oldIndex, int newIndex) override {}

    void valueTreeParentChanged (juce::ValueTree& treeWhoseParentHasChanged) override {}


    MagicBuilder&       builder;
    juce::UndoManager&  undo;

    juce::ComboBox      nodeSelect;

    PropertiesListModel propertiesModel { *this };
    juce::ListBox       propertiesList  { {}, &propertiesModel };

    juce::ComboBox      propertySelect;
    juce::TextButton    propertyAdd { "+" };

    juce::ValueTree     style;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertiesEditor)
};

} // namespace foleys
