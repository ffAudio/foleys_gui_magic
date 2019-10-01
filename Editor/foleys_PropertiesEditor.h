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

class PropertiesEditor  : public juce::Component
{
public:
    PropertiesEditor();

    void setStyle (juce::ValueTree style);
    void setColourNames (juce::StringArray names);

    void resized() override;

private:

    void updatePopupMenu();

    class PropertiesItem : public juce::Component
    {
    public:
        PropertiesItem();

        void setProperty (const juce::String& name, const juce::Value& propertyValue);
        void paint (juce::Graphics&) override;
        void resized() override;

    private:
        juce::String name;
        juce::Label  value;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertiesItem)
    };

    class PropertiesListModel : public juce::ListBoxModel
    {
    public:
        PropertiesListModel()=default;

        void setNodeToEdit (juce::ValueTree node);

        void setColourNames (juce::StringArray names);

        int getNumRows() override;

        juce::Component* refreshComponentForRow (int rowNumber,
                                                 bool isRowSelected,
                                                 juce::Component *existingComponentToUpdate) override;

        void paintListBoxItem (int rowNumber,
                               juce::Graphics& g,
                               int width, int height,
                               bool rowIsSelected) override {};

    private:

        juce::ValueTree   styleItem;
        juce::StringArray colourNames;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertiesListModel)
    };

    juce::ComboBox      nodeSelect;

    PropertiesListModel propertiesModel;
    juce::ListBox       propertiesList { {}, &propertiesModel };

    juce::ValueTree     style;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertiesEditor)
};

} // namespace foleys
