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
 The Decorator class will draw borders and descriptions around widgets, if defined.
 It also owns the Component and the Attachment, in case the Component is connected
 to an AudioProcessorValueTreeState.
 */
class Decorator   : public juce::Component,
                    private juce::Value::Listener
#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
                , public juce::DragAndDropTarget
#endif
{
public:
    Decorator (MagicGUIBuilder& builder, juce::ValueTree node, std::unique_ptr<juce::Component> wrapped = {});

    /**
     This method sets the GUI in edit mode, that allows to drag the components around.
     */
    virtual void setEditMode (bool shouldEdit);

    /**
     This will get the necessary information from the stylesheet, using inheritance
     of nodes if needed, to set the margins/borders etc. for the Decorator.
     */
    void configureDecorator (Stylesheet& stylesheet, const juce::ValueTree& node);

    /**
     This will get the necessary information from the stylesheet, using inheritance
     of nodes if needed, to set specific properties for the wrapped component.
     */
    void configureComponent (Stylesheet& stylesheet, const juce::ValueTree& node);

    void paint (juce::Graphics& g) override;
    void resized() override;

    virtual bool isContainer() const { return false; }

    /**
     This will trigger a recalculation of the children layout regardless of resized
     */
    virtual void updateLayout();

    void configureFlexBoxItem (const juce::ValueTree& node);

    /**
     Allows accessing the Component inside that Decorator. Don't keep this pointer!
     */
    juce::Component* getWrappedComponent();

    const juce::ValueTree& getConfigNode() const;

    juce::Colour backgroundColour { juce::Colours::darkgrey };
    juce::Colour borderColour     { juce::Colours::silver };

    float margin  = 5.0f;
    float padding = 5.0f;
    float border  = 0.0f;
    float radius  = 5.0f;

    juce::String        caption;
    juce::Justification justification = juce::Justification::centredTop;
    float               captionSize   = 20.0f;
    juce::Colour        captionColour = juce::Colours::silver;

    juce::String        tabCaption;
    juce::Colour        tabColour;

    juce::FlexItem flexItem { juce::FlexItem (*this).withFlex (1.0f) };

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    void paintOverChildren (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& event) override;

    void mouseDrag (const juce::MouseEvent& event) override;

    bool isInterestedInDragSource (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;
    void itemDropped (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;
#endif

protected:

    juce::Rectangle<int> getClientBounds() const;

    MagicGUIBuilder&            magicBuilder;

private:

    void valueChanged (juce::Value& source) override;

    juce::ValueTree             configNode;
    juce::Value                 visibility { true };

    std::unique_ptr<juce::Component> component;

    juce::Image                 backgroundImage;
    float                       backgroundAlpha = 1.0f;
    juce::RectanglePlacement    backgroundPlacement = juce::RectanglePlacement::centred;
    juce::Array<juce::Colour>   backgroundFill;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Decorator)
};

} // namespace foleys
