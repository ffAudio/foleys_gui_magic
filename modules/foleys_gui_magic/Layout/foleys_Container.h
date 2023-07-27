/*
 ==============================================================================
    Copyright (c) 2019-2023 Foleys Finest Audio - Daniel Walz
    All rights reserved.

    **BSD 3-Clause License**

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

 ==============================================================================

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

#include "foleys_GuiItem.h"

namespace foleys
{

class MagicPlotComponent;

/**
 The LayoutType defines after which method
 */
enum class LayoutType
{
    Contents,
    FlexBox,
    Tabbed
};

/**
 The scroll mode
 */
enum class ScrollMode
{
    NoScroll,
    ScrollHorizontal,
    ScrollVertical,
    ScrollBoth
};

/**
 The Container is a GuiItem, that can hold multiple Components.
 In the editor it is seen as "View". With the setting "display"
 the layout strategy can be chosen.
 */
class Container   : public GuiItem,
                    private juce::ChangeListener,
                    private juce::Timer
{
public:
    Container (MagicGUIBuilder& builder, juce::ValueTree node);
    ~Container() override;

    /**
     Updates the layout fo children
     */
    void update() override;

    /**
     Append a child item. To change the order the flexbox.order is used.
     */
    void addChildItem (std::unique_ptr<GuiItem> child);

    std::vector<std::unique_ptr<GuiItem>>::iterator begin();
    std::vector<std::unique_ptr<GuiItem>>::iterator end();

    /**
     Sets the layout mode of the container
     */
    void setLayoutMode (LayoutType layout);

    /**
     Returns the current layout mode
     */
    LayoutType getLayoutMode() const;

    void resized() override;

    bool isContainer() const override { return true; }

    void createSubComponents() override;

    /**
     This will trigger a recalculation of the children layout regardless of resized
     */
    void updateLayout() override;

    void updateColours() override;

    void updateContinuousRedraw();

    void configureFlexBox (const juce::ValueTree& node);

    juce::Component* getWrappedComponent() override { return nullptr; }

    /**
     Lookup a Component through the tree. It will return the first with that id regardless if there is another one.
     We discourage using that function, because that Component can be deleted and recreated at any time without notice.
     */
    GuiItem* findGuiItemWithId (const juce::String& name) override;

    /**
     Seeks recursively for a GuiItem
     */
    GuiItem* findGuiItem (const juce::ValueTree& node) override;

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    /**
     This switches this node and all it's descendents in the edit
     mode, which means, the components don't react, but instead you
     can move them around.
     */
    void setEditMode (bool shouldEdit) override;
#endif

private:
    class Scroller : public juce::Viewport
    {
    public:
        Scroller (Container& owner);

        void paint (juce::Graphics& g) override;
        void setBackgroundColour (juce::Colour colour);

    private:
        Container& owner;
        juce::Colour backgroundColour;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Scroller)
    };

    void changeListenerCallback (juce::ChangeBroadcaster*) override;
    void valueChanged (juce::Value&) override;
    void timerCallback() override;

    void updateTabbedButtons();
    void updateSelectedTab();

    juce::Value   currentTab { juce::var {0} };
    int           tabbarHeight  = 30;
    int           refreshRateHz = 30;
    LayoutType    layout = LayoutType::FlexBox;
    juce::FlexBox flexBox;
    ScrollMode    scrollMode = ScrollMode::NoScroll;

    juce::Component                         containerBox;
    Scroller                                viewport { *this };
    std::unique_ptr<juce::TabbedButtonBar>  tabbedButtons;
    std::vector<std::unique_ptr<GuiItem>>   children;

    std::vector<juce::Component::SafePointer<MagicPlotComponent>> plotComponents;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Container)
};

} // namespace foleys
