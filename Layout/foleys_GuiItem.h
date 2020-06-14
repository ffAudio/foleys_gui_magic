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
class MagicGUIState;

/**
 The GuiItem class will draw borders and descriptions around widgets, if defined.
 It also owns the Component and the Attachment, in case the Component is connected
 to an AudioProcessorValueTreeState.
 */
class GuiItem   : public juce::Component,
                  private juce::Value::Listener,
                  private juce::ValueTree::Listener
#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
                , public juce::DragAndDropTarget
#endif
{
public:
    GuiItem (MagicGUIBuilder& builder, juce::ValueTree node);

    /**
     Allows accessing the Component inside that GuiItem. Don't keep this pointer!
     */
    virtual juce::Component* getWrappedComponent() = 0;

    /**
     In update() the ValueTree properties should be used to set all properties to the component.
     You can use the magicBuilder to resolve properties from CSS.
     The Colours will be handled by default.
     */
    virtual void update() = 0;

    /**
     Override this to return each settable option the designer should be able to configure on your component.
     */
    virtual std::vector<SettableProperty> getSettableProperties() const { return {}; }

    /**
     For each factory you can register a translation table, which will forward the colours from the
     Stylesheet to the Components.
     */
    void setColourTranslation (std::vector<std::pair<juce::String, int>> mapping);

    /**
     Return the names of configurable colours
     */
    juce::StringArray getColourNames() const;

    /**
     Look up a value through the DOM and CSS
     */
    juce::var getProperty (const juce::Identifier& property);

    MagicGUIState& getMagicState();

    /**
     Reread properties from the config ValueTree
     */
    void updateInternal();

    void paint (juce::Graphics& g) final;
    void resized() override;

    virtual bool isContainer() const { return false; }

    virtual void createSubComponents() {}

    /**
     This will trigger a recalculation of the children layout regardless of resized
     */
    virtual void updateLayout();

    void configureFlexBoxItem (const juce::ValueTree& node);


    juce::Rectangle<int> getClientBounds() const;

    juce::String getTabCaption (const juce::String& defaultName) const;
    juce::Colour getTabColour() const;

    const juce::ValueTree& getConfigNode() const;

    juce::FlexItem& getFlexItem() { return flexItem; };

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    /**
     This method sets the GUI in edit mode, that allows to drag the components around.
     */
    virtual void setEditMode (bool shouldEdit);

    void paintOverChildren (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& event) override;

    void mouseDrag (const juce::MouseEvent& event) override;

    bool isInterestedInDragSource (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;
    void itemDropped (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;
#endif

    MagicGUIBuilder& magicBuilder;

protected:

    juce::ValueTree configNode;

    juce::FlexItem flexItem { juce::FlexItem (*this).withFlex (1.0f) };

    std::vector<std::pair<juce::String, int>> colourTranslation;

private:

    void valueChanged (juce::Value& source) override;

    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override;

    void valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&) override;

    void valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int) override;

    void valueTreeChildOrderChanged (juce::ValueTree&, int, int) override;

    void valueTreeParentChanged (juce::ValueTree&) override;

    /**
     This will get the necessary information from the stylesheet, using inheritance
     of nodes if needed, to set specific properties for the wrapped component.
     */
    void configureComponent();

    juce::Value     visibility { true };

    Decorator       decorator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiItem)
};

} // namespace foleys
