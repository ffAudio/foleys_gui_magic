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

class SliderItem : public GuiItem
{

public:
    FOLEYS_DECLARE_GUI_FACTORY (SliderItem)

    static const juce::Identifier  pSliderType;
    static const juce::StringArray pSliderTypes;

    static const juce::Identifier  pSliderTextBox;
    static const juce::StringArray pTextBoxPositions;

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
        attachment.reset();
        auto paramID = configNode.getProperty (IDs::parameter, juce::String()).toString();
        if (paramID.isNotEmpty() && magicBuilder.getProcessorState())
            attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(magicBuilder.getProcessorState()->getValueTreeState(), paramID, slider);

        auto type = getProperty (pSliderType).toString();
        slider.setAutoOrientation (type.isEmpty() || type == pSliderTypes [0]);

        if (type == pSliderTypes [1])
            slider.setSliderStyle (juce::Slider::LinearHorizontal);
        else if (type == pSliderTypes [2])
            slider.setSliderStyle (juce::Slider::LinearVertical);
        else if (type == pSliderTypes [3])
            slider.setSliderStyle (juce::Slider::Rotary);
        else if (type == pSliderTypes [4])
            slider.setSliderStyle (juce::Slider::IncDecButtons);

        auto textbox = getProperty (pSliderTextBox).toString();
        if (textbox == pTextBoxPositions [0])
            slider.setTextBoxStyle (juce::Slider::NoTextBox, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
        else if (textbox == pTextBoxPositions [1])
            slider.setTextBoxStyle (juce::Slider::TextBoxAbove, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
        else if (textbox == pTextBoxPositions [3])
            slider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
        else if (textbox == pTextBoxPositions [4])
            slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
        else
            slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> properties;

        properties.push_back ({ configNode, IDs::parameter, SettableProperty::Choice, {}, magicBuilder.createParameterMenu() });
        properties.push_back ({ configNode, pSliderType, SettableProperty::Choice, pSliderTypes [0], makePopupMenu (pSliderTypes) });
        properties.push_back ({ configNode, pSliderTextBox, SettableProperty::Choice, pTextBoxPositions [2], makePopupMenu (pTextBoxPositions) });

        return properties;
    }

    juce::Component* getWrappedComponent() override
    {
        return &slider;
    }

private:
    AutoOrientationSlider slider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderItem)
};
const juce::Identifier  SliderItem::pSliderType   { "slider-type" };
const juce::StringArray SliderItem::pSliderTypes  { "auto", "linear-horizontal", "linear-vertical", "rotary", "inc-dec-buttons" };
const juce::Identifier  SliderItem::pSliderTextBox    { "slider-textbox" };
const juce::StringArray SliderItem::pTextBoxPositions { "no-textbox", "textbox-above", "textbox-below", "textbox-left", "textbox-right" };


//==============================================================================

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

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> properties;
        properties.push_back ({ configNode, IDs::parameter, SettableProperty::Choice, {}, magicBuilder.createParameterMenu() });
        return properties;
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

//==============================================================================

class TextButtonItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (TextButtonItem)

    static const juce::Identifier pText;
    static const juce::Identifier pOnClick;

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

        button.setButtonText (magicBuilder.getStyleProperty (pText, configNode));

        if (auto* state = magicBuilder.getProcessorState())
        {
            auto triggerID = getProperty (pOnClick).toString();
            if (triggerID.isNotEmpty())
                button.onClick = state->getTrigger (triggerID);
        }
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> properties;

        properties.push_back ({ configNode, IDs::parameter, SettableProperty::Choice, {}, magicBuilder.createParameterMenu() });
        properties.push_back ({ configNode, pText, SettableProperty::Text, {}, {} });
        properties.push_back ({ configNode, pOnClick, SettableProperty::Choice, {}, magicBuilder.createTriggerMenu() });

        return properties;
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
const juce::Identifier TextButtonItem::pText    { "text" };
const juce::Identifier TextButtonItem::pOnClick { "onClick" };


//==============================================================================

class ToggleButtonItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (ToggleButtonItem)

    static const juce::Identifier pText;
    static const juce::Identifier pValue;

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

        button.setButtonText (magicBuilder.getStyleProperty (pText, configNode));

        if (auto* state = magicBuilder.getProcessorState())
        {
            auto propertyID = getProperty (pValue).toString();
            if (propertyID.isNotEmpty())
                button.getToggleStateValue().referTo (state->getPropertyAsValue (propertyID));
        }
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> properties;
        properties.push_back ({ configNode, IDs::parameter, SettableProperty::Choice, {}, magicBuilder.createParameterMenu() });
        return properties;
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
const juce::Identifier ToggleButtonItem::pText  { "text" };
const juce::Identifier ToggleButtonItem::pValue { "value" };


//==============================================================================

class LabelItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (LabelItem)

    static const juce::Identifier  pText;
    static const juce::Identifier  pJustification;
    static const juce::Identifier  pFontSize;
    static const juce::Identifier  pEditable;
    static const juce::Identifier  pValue;

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
        label.setText (magicBuilder.getStyleProperty (pText, configNode), juce::dontSendNotification);

        auto justifications = makeJustificationsChoices();
        auto justification = getProperty (pJustification).toString();
        if (justification.isNotEmpty())
            label.setJustificationType (juce::Justification (justifications.getWithDefault (justification, juce::Justification::centredLeft)));
        else
            label.setJustificationType (juce::Justification::centredLeft);

        label.setFont (juce::Font (getProperty (pFontSize)));

        label.setEditable (getProperty (pEditable));

        if (auto* state = magicBuilder.getProcessorState())
        {
            auto propertyPath = getProperty (pValue).toString();
            if (propertyPath.isNotEmpty())
                label.getTextValue().referTo (state->getPropertyAsValue (propertyPath));
        }
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> properties;
        properties.push_back ({ configNode, pText, SettableProperty::Text, {}, {} });
        properties.push_back ({ configNode, pJustification, SettableProperty::Choice, {}, makePopupMenu (getAllKeyNames (makeJustificationsChoices())) });
        properties.push_back ({ configNode, pFontSize, SettableProperty::Number, {}, {} });
        properties.push_back ({ configNode, pEditable, SettableProperty::Toggle, {}, {} });
        properties.push_back ({ configNode, pValue, SettableProperty::Property, {}, {} });
        return properties;
    }

    juce::Component* getWrappedComponent() override
    {
        return &label;
    }

private:
    juce::Label label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabelItem)
};
const juce::Identifier  LabelItem::pText            { "text" };
const juce::Identifier  LabelItem::pJustification   { "justification" };
const juce::Identifier  LabelItem::pFontSize        { "font-size" };
const juce::Identifier  LabelItem::pEditable        { "editable" };
const juce::Identifier  LabelItem::pValue           { "value" };

//==============================================================================

class PlotItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (PlotItem)

    static const juce::Identifier  pDecay;

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

        auto decay = double (getProperty (pDecay));
        plot.setDecayFactor (decay);
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> properties;
        properties.push_back ({ configNode, IDs::source, SettableProperty::Choice, {}, magicBuilder.createObjectMenu<MagicPlotSource>() });
        properties.push_back ({ configNode, pDecay,      SettableProperty::Number, {}, {} });
        return properties;
    }

    juce::Component* getWrappedComponent() override
    {
        return &plot;
    }

private:
    MagicPlotComponent plot;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlotItem)
};
const juce::Identifier  PlotItem::pDecay {"plot-decay"};

//==============================================================================

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

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> properties;

        properties.push_back ({ configNode, IDs::parameterX, SettableProperty::Choice, {}, magicBuilder.createParameterMenu() });
        properties.push_back ({ configNode, IDs::parameterY, SettableProperty::Choice, {}, magicBuilder.createParameterMenu() });
        properties.push_back ({ configNode, "right-click", SettableProperty::Choice, {}, magicBuilder.createParameterMenu() });
        properties.push_back ({ configNode, "xy-crosshair", SettableProperty::Choice, {}, makePopupMenu ({  }) });

        return properties;
    }

    juce::Component* getWrappedComponent() override
    {
        return &dragger;
    }

private:
    XYDragComponent dragger;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYDraggerItem)
};

//==============================================================================

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

//==============================================================================

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

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> properties;
        properties.push_back ({ configNode, IDs::source, SettableProperty::Choice, {}, magicBuilder.createObjectMenu<MagicLevelSource>() });
        return properties;
    }

    juce::Component* getWrappedComponent() override
    {
        return &meter;
    }

private:
    MagicLevelMeter meter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeterItem)
};

//==============================================================================

class ListBoxItem : public GuiItem,
                    public juce::ChangeListener
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
        {
            if (auto* m = dynamic_cast<juce::ChangeBroadcaster*>(listBox.getModel()))
                m->removeChangeListener (this);

            auto modelID = configNode.getProperty ("list-box-model", juce::String()).toString();
            if (modelID.isNotEmpty())
            {
                if (auto* model = processorState->getObjectWithType<juce::ListBoxModel>(modelID))
                {
                    listBox.setModel (model);
                    if (auto* m = dynamic_cast<juce::ChangeBroadcaster*>(model))
                        m->addChangeListener (this);
                }
            }
            else
            {
                listBox.setModel (nullptr);
            }
        }
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> properties;
        properties.push_back ({ configNode, "list-box-model", SettableProperty::Choice, {}, magicBuilder.createObjectMenu<juce::ListBoxModel>() });
        return properties;
    }

    juce::Component* getWrappedComponent() override
    {
        return &listBox;
    }

    void changeListenerCallback (juce::ChangeBroadcaster*) override
    {
        listBox.updateContent();
    }

private:
    juce::ListBox listBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListBoxItem)
};

//==============================================================================

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
