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


class DummyItem  : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (DummyItem)

    DummyItem (MagicGUIBuilder& builder, const juce::ValueTree& node) : GuiItem (builder, node)
    {
        addAndMakeVisible (label);
    }

    juce::Component* getWrappedComponent() override
    {
        return &label;
    }

private:
    juce::Label label { {}, "Dummy" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DummyItem)
};

class SliderItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (SliderItem)

    SliderItem (MagicGUIBuilder& builder, const juce::ValueTree& node) : GuiItem (builder, node)
    {
        setColourTranslation (
        {
            { "slider-background", juce::Slider::backgroundColourId },
            { "slider-thumb", juce::Slider::thumbColourId },
            { "slider-track", juce::Slider::trackColourId },
            { "rotary-fill", juce::Slider::rotarySliderFillColourId },
            { "rotary-outline", juce::Slider::rotarySliderOutlineColourId },
            { "slider-text", juce::Slider::textBoxTextColourId },
            { "slider-text-background", juce::Slider::textBoxBackgroundColourId },
            { "slider-text-highlight", juce::Slider::textBoxHighlightColourId },
            { "slider-text-outline", juce::Slider::textBoxOutlineColourId }
        });

        addAndMakeVisible (slider);
    }

    void update() override
    {
        auto parameter = configNode.getProperty (IDs::parameter, juce::String()).toString();
        if (parameter.isNotEmpty() && magicBuilder.getProcessorState())
            slider.attachToParameter (parameter, magicBuilder.getProcessorState()->getValueTreeState());

        auto type = getProperty ("slider-type").toString();
        slider.setAutoOrientation (type.isEmpty() || type == "auto");
        if (type == "linear-horizontal")
            slider.setSliderStyle (juce::Slider::LinearHorizontal);
        else if (type == "linear-vertical")
            slider.setSliderStyle (juce::Slider::LinearVertical);
        else if (type == "rotary")
            slider.setSliderStyle (juce::Slider::Rotary);
        else if (type == "inc-dec-buttons")
            slider.setSliderStyle (juce::Slider::IncDecButtons);

        auto textbox = getProperty ("slider-textbox").toString();
        if (textbox == "no-textbox")
            slider.setTextBoxStyle (juce::Slider::NoTextBox, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
        else if (textbox == "textbox-left")
            slider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
        else if (textbox == "textbox-right")
            slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
        else if (textbox == "textbox-above")
            slider.setTextBoxStyle (juce::Slider::TextBoxAbove, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
        else
            slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
    }

    juce::Component* getWrappedComponent() override
    {
        return &slider;
    }

private:
    AttachableSlider slider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderItem)
};

class ComboBoxItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (ComboBoxItem)

    ComboBoxItem (MagicGUIBuilder& builder, const juce::ValueTree& node) : GuiItem (builder, node)
    {
        setColourTranslation (
        {
            { "combo-background", juce::ComboBox::backgroundColourId },
            { "combo-text", juce::ComboBox::textColourId },
            { "combo-outline", juce::ComboBox::outlineColourId },
            { "combo-button", juce::ComboBox::buttonColourId },
            { "combo-arrow", juce::ComboBox::arrowColourId },
            { "combo-focused-outline", juce::ComboBox::focusedOutlineColourId },
            { "combo-menu-background", juce::PopupMenu::backgroundColourId },
            { "combo-menu-background-highlight", juce::PopupMenu::highlightedBackgroundColourId },
            { "combo-menu-text", juce::PopupMenu::textColourId },
            { "combo-menu-text-highlight", juce::PopupMenu::highlightedTextColourId }
        });

        addAndMakeVisible (comboBox);
    }

    void update() override
    {
        if (auto* state = magicBuilder.getProcessorState())
        {
            auto paramID = configNode.getProperty (IDs::parameter, juce::String()).toString();
            if (paramID.isNotEmpty())
            {
                auto* parameter = state->getValueTreeState().getParameter (paramID);
                attachment.reset();
                comboBox.clear();
                comboBox.addItemList (parameter->getAllValueStrings(), 1);
                attachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state->getValueTreeState(), paramID, comboBox);
            }
            else
            {
                attachment.reset();
            }
        }
    }

    juce::Component* getWrappedComponent() override
    {
        return &comboBox;
    }

private:
    juce::ComboBox comboBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComboBoxItem)
};

class TextButtonItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (TextButtonItem)

    TextButtonItem (MagicGUIBuilder& builder, const juce::ValueTree& node) : GuiItem (builder, node)
    {
        setColourTranslation (
        {
            { "button-color", juce::TextButton::buttonColourId },
            { "button-on-color", juce::TextButton::buttonOnColourId },
            { "button-off-text", juce::TextButton::textColourOffId },
            { "button-on-text", juce::TextButton::textColourOnId }
        });

        addAndMakeVisible (button);
    }

    void update() override
    {
        auto parameter = configNode.getProperty (IDs::parameter, juce::String()).toString();
        if (parameter.isNotEmpty() && magicBuilder.getProcessorState())
        {
            attachment.reset();
            attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(magicBuilder.getProcessorState()->getValueTreeState(), parameter, button);
        }
        else
        {
            attachment.reset();
        }

        button.setButtonText (magicBuilder.getStyleProperty ("text", configNode));

        if (auto* state = magicBuilder.getProcessorState())
            button.onClick = state->getTrigger (getProperty ("onClick").toString());
    }

    juce::Component* getWrappedComponent() override
    {
        return &button;
    }

private:
    juce::TextButton button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextButtonItem)
};

class ToggleButtonItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (ToggleButtonItem)

    ToggleButtonItem (MagicGUIBuilder& builder, const juce::ValueTree& node) : GuiItem (builder, node)
    {
        setColourTranslation (
        {
            { "toggle-text", juce::ToggleButton::textColourId },
            { "toggle-tick", juce::ToggleButton::tickColourId },
            { "toggle-tick-disabled", juce::ToggleButton::tickDisabledColourId }
        });

        addAndMakeVisible (button);
    }

    void update() override
    {
        auto parameter = configNode.getProperty (IDs::parameter, juce::String()).toString();
        if (parameter.isNotEmpty() && magicBuilder.getProcessorState())
        {
            attachment.reset();
            attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(magicBuilder.getProcessorState()->getValueTreeState(), parameter, button);
        }
        else
        {
            attachment.reset();
        }

        button.setButtonText (magicBuilder.getStyleProperty ("text", configNode));

        if (auto* state = magicBuilder.getProcessorState())
        {
            auto propertyID = getProperty ("value").toString();
            if (propertyID.isNotEmpty())
                button.getToggleStateValue().referTo (state->getPropertyAsValue (propertyID));
        }
    }

    juce::Component* getWrappedComponent() override
    {
        return &button;
    }

private:
    juce::ToggleButton button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToggleButtonItem)
};

class LabelItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (LabelItem)

    LabelItem (MagicGUIBuilder& builder, const juce::ValueTree& node) : GuiItem (builder, node)
    {
        setColourTranslation (
        {
            { "label-background",         juce::Label::backgroundColourId },
            { "label-outline",            juce::Label::outlineColourId },
            { "label-text",               juce::Label::textColourId },
            { "label-editing-background", juce::Label::backgroundWhenEditingColourId },
            { "label-editing-outline",    juce::Label::outlineWhenEditingColourId },
            { "label-editing-text",       juce::Label::textWhenEditingColourId }
        });

        addAndMakeVisible (label);
    }

    void update() override
    {
        label.setText (magicBuilder.getStyleProperty ("text", configNode), juce::dontSendNotification);

        auto justifications = MagicGUIBuilder::makeJustificationsChoices();
        label.setJustificationType (juce::Justification (justifications.getWithDefault (getProperty ("justification").toString(), juce::Justification::centredLeft)));

        label.setFont (juce::Font (getProperty ("font-size")));

        label.setEditable (getProperty ("editable"));

        if (auto* state = magicBuilder.getProcessorState())
            label.getTextValue().referTo (state->getPropertyAsValue (getProperty ("value").toString()));
    }

    juce::Component* getWrappedComponent() override
    {
        return &label;
    }

private:
    juce::Label label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabelItem)
};

class PlotItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (PlotItem)

    PlotItem (MagicGUIBuilder& builder, const juce::ValueTree& node) : GuiItem (builder, node)
    {
        setColourTranslation (
        {
            { "plot-color", MagicPlotComponent::plotColourId },
            { "plot-fill-color", MagicPlotComponent::plotFillColourId },
            { "plot-inactive-color", MagicPlotComponent::plotInactiveColourId },
            { "plot-inactive-fill-color", MagicPlotComponent::plotInactiveFillColourId }
        });

        addAndMakeVisible (plot);
    }

    void update() override
    {
        if (auto* processorState = magicBuilder.getProcessorState())
        {
            auto sourceID = configNode.getProperty (IDs::source, juce::String()).toString();
            if (sourceID.isNotEmpty())
                plot.setPlotSource (processorState->getObjectWithType<MagicPlotSource>(sourceID));
        }

        auto decay = double (getProperty ("plot-decay"));
        plot.setDecayFactor (decay);
    }

    juce::Component* getWrappedComponent() override
    {
        return &plot;
    }

private:
    MagicPlotComponent plot;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlotItem)
};

class XYDraggerItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (XYDraggerItem)

    XYDraggerItem (MagicGUIBuilder& builder, const juce::ValueTree& node)
      : GuiItem (builder, node),
        dragger (builder.getProcessorState()->getValueTreeState())
    {
        setColourTranslation (
        {
            { "xy-drag-handle",      XYDragComponent::xyDotColourId },
            { "xy-drag-handle-over", XYDragComponent::xyDotOverColourId },
            { "xy-horizontal",       XYDragComponent::xyHorizontalColourId },
            { "xy-horizontal-over",  XYDragComponent::xyHorizontalOverColourId },
            { "xy-vertical",         XYDragComponent::xyVerticalColourId },
            { "xy-vertical-over",    XYDragComponent::xyVerticalOverColourId }
        });

        addAndMakeVisible (dragger);
    }

    void update() override
    {
        dragger.setParameterX (configNode.getProperty (IDs::parameterX, juce::String()).toString());
        dragger.setParameterY (configNode.getProperty (IDs::parameterY, juce::String()).toString());

        if (auto* state = magicBuilder.getProcessorState())
            dragger.setRightClickParameter (configNode.getProperty ("right-click", juce::String()), state->getValueTreeState());

        auto crosshair = getProperty ("xy-crosshair");
        if (crosshair == "no-crosshair")
            dragger.setCrossHair (false, false);
        else if (crosshair == "vertical")
            dragger.setCrossHair (true, false);
        else if (crosshair == "horizontal")
            dragger.setCrossHair (false, true);
        else
            dragger.setCrossHair (true, true);
    }

    juce::Component* getWrappedComponent() override
    {
        return &dragger;
    }

private:
    XYDragComponent dragger;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYDraggerItem)
};

class KeyboardItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (KeyboardItem)

    KeyboardItem (MagicGUIBuilder& builder, const juce::ValueTree& node)
      : GuiItem (builder, node),
        keyboard (builder.getProcessorState()->getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
    {
        setColourTranslation (
        {
            { "white-note-color",      juce::MidiKeyboardComponent::whiteNoteColourId },
            { "black-note-color",      juce::MidiKeyboardComponent::blackNoteColourId },
            { "key-separator-line-color", juce::MidiKeyboardComponent::keySeparatorLineColourId },
            { "mouse-over-color",      juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId },
            { "key-down-color",        juce::MidiKeyboardComponent::keyDownOverlayColourId },
        });

        addAndMakeVisible (keyboard);
    }

    void update() override
    {
        auto size = getProperty ("key-width");
        keyboard.setKeyWidth (size.isVoid() ? 50.0f : float (size));

        auto orientation = getProperty ("orientation").toString();
        if (orientation == "vertical-left")
            keyboard.setOrientation (juce::MidiKeyboardComponent::verticalKeyboardFacingLeft);
        else if (orientation == "vertical-right")
            keyboard.setOrientation (juce::MidiKeyboardComponent::verticalKeyboardFacingRight);
        else
            keyboard.setOrientation (juce::MidiKeyboardComponent::horizontalKeyboard);
    }

    juce::Component* getWrappedComponent() override
    {
        return &keyboard;
    }

private:
    juce::MidiKeyboardComponent keyboard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeyboardItem)
};

class LevelMeterItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (LevelMeterItem)

    LevelMeterItem (MagicGUIBuilder& builder, const juce::ValueTree& node) : GuiItem (builder, node)
    {
        setColourTranslation (
        {
            { "background-color", MagicLevelMeter::backgroundColourId },
            { "bar-background-color", MagicLevelMeter::barBackgroundColourId },
            { "outline-color", MagicLevelMeter::outlineColourId },
            { "bar-fill-color", MagicLevelMeter::barFillColourId },
            { "tickmark-color", MagicLevelMeter::tickmarkColourId },
        });

        addAndMakeVisible (meter);
    }

    void update() override
    {
        if (auto* processorState = magicBuilder.getProcessorState())
        {
            auto sourceID = configNode.getProperty (IDs::source, juce::String()).toString();
            if (sourceID.isNotEmpty())
                meter.setLevelSource (processorState->getObjectWithType<MagicLevelSource>(sourceID));
        }
    }

    juce::Component* getWrappedComponent() override
    {
        return &meter;
    }

private:
    MagicLevelMeter meter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeterItem)
};

class ListBoxItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (ListBoxItem)

    ListBoxItem (MagicGUIBuilder& builder, const juce::ValueTree& node) : GuiItem (builder, node)
    {
        addAndMakeVisible (listBox);
    }

    void update() override
    {
        if (auto* processorState = magicBuilder.getProcessorState())
            if (auto* model = processorState->getObjectWithType<juce::ListBoxModel>(configNode.getProperty ("list-box-model", juce::String()).toString()))
                listBox.setModel (model);
    }

    juce::Component* getWrappedComponent() override
    {
        return &listBox;
    }

private:
    juce::ListBox listBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListBoxItem)
};

#if JUCE_MODULE_AVAILABLE_juce_gui_extra && JUCE_WEB_BROWSER
class WebBrowserItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (WebBrowserItem)

    WebBrowserItem (MagicGUIBuilder& builder, const juce::ValueTree& node) : GuiItem (builder, node)
    {
        addAndMakeVisible (browser);
    }

    void update() override
    {
        browser.goToURL (getProperty ("url").toString());
    }

    juce::Component* getWrappedComponent() override
    {
        return &browser;
    }

private:
    juce::WebBrowserComponent browser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WebBrowserItem)
};
#endif // JUCE_WEB_BROWSER

void MagicGUIBuilder::registerJUCEFactories()
{
    registerFactory (IDs::dummyItem, &DummyItem::factory);
    registerFactory (IDs::slider, &SliderItem::factory);
    registerFactory (IDs::comboBox, &ComboBoxItem::factory);
    registerFactory (IDs::textButton, &TextButtonItem::factory);
    registerFactory (IDs::toggleButton, &ToggleButtonItem::factory);
    registerFactory (IDs::label, &LabelItem::factory);
    registerFactory (IDs::plot, &PlotItem::factory);
    registerFactory (IDs::xyDragComponent, &XYDraggerItem::factory);
    registerFactory (IDs::keyboardComponent, &KeyboardItem::factory);
    registerFactory (IDs::meter, &LevelMeterItem::factory);
    registerFactory (IDs::listBox, &ListBoxItem::factory);

#if JUCE_MODULE_AVAILABLE_juce_gui_extra && JUCE_WEB_BROWSER
    registerFactory (IDs::webBrowser, &WebBrowserItem::factory);
#endif // JUCE_WEB_BROWSER
}

} // namespace foleys
