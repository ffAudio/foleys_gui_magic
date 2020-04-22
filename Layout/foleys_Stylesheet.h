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

class MagicGUIBuilder;

/**
 The Stylesheet class represents all style information. It is organised in
 types, which corresponds to the Components created, classes that can be
 referenced by any node, and nodes referenced by id.
 */
class Stylesheet
{
public:

    Stylesheet() = default;

    /**
     The Stylesheet node in the XML can contain several Styles to select from.
     Use this method to select a style.
     */
    void setStyle (const juce::ValueTree& node);

    /**
     Set the size of the UI. This will be used to resolve the properties from the style classes.
     @param width is the width of the UI
     @param height is the height of the UI
     @return false, if the properties are likely to have changed (because a conditional class became valid or invalid)
     */
    bool setMediaSize (int width, int height);

    /**
     Reads the range, in which the properties won't change due to conditional classes
     */
    void updateValidRanges();

    /**
     Read the style classes and connect variables

     @param builder is the managed builder instance, so the style class can connect to the variables
     */
    void updateStyleClasses (MagicGUIBuilder& builder);

    /**
     This method traverses the dom and checks each style, if that property was defined.

     @param name the name of the property.
     @param node is the node in the DOM. This is used for inheritance by traversing upwards.
     */
    juce::var getStyleProperty (const juce::Identifier& name, const juce::ValueTree& node, bool inherit=true, juce::ValueTree* definedHere=nullptr) const;

    /**
     Return the LookAndFeel for the node. Make sure never to remove a LookAndFeel, especially
     as long as the ComponentTree is still referencing any of them.

     @param node is the node in the GUI DOM
     */
    juce::LookAndFeel* getLookAndFeel (const juce::ValueTree& node) const;

    /**
     Finds a background image for the given node. Note that this will only return anything
     useful, if you have added any actual images into the BinaryData and have enabled the
     FOLEYS_ENABLE_BINARY_DATA setting in the module page.

     @param node is the node in the GUI DOM
     */
    juce::Image getBackgroundImage (const juce::ValueTree& node) const;

    juce::Array<juce::Colour> getBackgroundGradient (const juce::ValueTree& node) const;

    /**
     This is a slightly more intelligent colour lookup than the JUCE one, as it allows to
     use colour names, as well as 6 digit colour tuples (JUCE will use red as alpha in that case)

     @param name a string representing the colour, can be an actual name or a RGB tuple or ARGB tuple.
     */
    static juce::Colour parseColour (const juce::String& name);

    juce::ValueTree getCurrentStyle() const;

    /**
     Add a new style class node, so elements of your GUI can inherit that class.

     @param name is the name of the new class. Make sure you only use XML conform identifiers, i.e. characters,
                 numbers but not starting with a number, dash and underscore only.
     @param undo the UndoManager to track the action
     */
    juce::ValueTree addNewStyleClass (const juce::String& name, juce::UndoManager* undo);

    /**
     Delete a style class. This will not delete references to the class.

     @param name the name of the class to be deleted.
     @param undo the UndoManager to track the action
     */
    void deleteStyleClass (const juce::String& name, juce::UndoManager* undo);

    /**
     With that method you can register your custom LookAndFeel class and apply it to different components.

     @param name the name the LookAndFeel can be referenced in the GUI editor
     @param lookAndFeel is an actual LookAndFeel instance, that is owned by the Stylesheet
     */
    void registerLookAndFeel (juce::String name, std::unique_ptr<juce::LookAndFeel> lookAndFeel);

    /**
     This creates a default stylesheet from scratch, to allow the default GUI to look sensible.
     */
    static juce::ValueTree createDefaultStyle();

    juce::StringArray getAllClassesNames() const;

    juce::StringArray getLookAndFeelNames() const;

    bool isClassNode (const juce::ValueTree& node) const;
    bool isTypeNode (const juce::ValueTree& node) const;
    bool isIdNode (const juce::ValueTree& node) const;

private:
    struct SizeRange
    {
        juce::Range<int> width  { 0, std::numeric_limits<int>::max() };
        juce::Range<int> height { 0, std::numeric_limits<int>::max() };
    };

    class StyleClass  : public juce::ChangeBroadcaster,
                        private juce::Value::Listener
    {
    public:
        StyleClass (juce::ValueTree style);

        void setActiveProperty (juce::Value& source);
        bool isActive() const;
        bool isRecursive() const;
        bool isValidForSize (int width, int height) const;

        SizeRange getValidSizeRange() const;

    private:
        void valueChanged (juce::Value &value) override;

        juce::ValueTree styleNode;

        juce::Value activeFlag { true };

        SizeRange   validRange;
        bool        recursive  { false };
    };

    juce::StringArray getParameters (const juce::String& text) const;

    juce::ValueTree   currentStyle;

    std::map<juce::String, std::unique_ptr<juce::LookAndFeel>> lookAndFeels;
    std::map<juce::String, std::unique_ptr<StyleClass>> styleClasses;

    int mediaWidth = 0;
    int mediaHeight = 0;

    SizeRange validMediaRanges;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Stylesheet)
};

} // namespace foleys
