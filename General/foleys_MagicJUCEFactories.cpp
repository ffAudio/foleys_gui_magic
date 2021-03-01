/*
 ==============================================================================
    Copyright (c) 2019-2021 Foleys Finest Audio - Daniel Walz
    All rights reserved.

    License for non-commercial projects:

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

    License for commercial products:

    To sell commercial products containing this module, you are required to buy a
    License from https://foleysfinest.com/developer/pluginguimagic/

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

    static const juce::Identifier  pValue;
    static const juce::Identifier  pMinValue;
    static const juce::Identifier  pMaxValue;

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

        auto type = getProperty (pSliderType).toString();
        slider.setAutoOrientation (type.isEmpty() || type == pSliderTypes [0]);

        if (type == pSliderTypes [1])
            slider.setSliderStyle (juce::Slider::LinearHorizontal);
        else if (type == pSliderTypes [2])
            slider.setSliderStyle (juce::Slider::LinearVertical);
        else if (type == pSliderTypes [3])
            slider.setSliderStyle (juce::Slider::Rotary);
        else if (type == pSliderTypes [4])
            slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        else if (type == pSliderTypes [5])
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

        double minValue = getProperty (pMinValue);
        double maxValue = getProperty (pMaxValue);
        if (maxValue > minValue)
            slider.setRange (minValue, maxValue);

        auto valueID = configNode.getProperty (pValue, juce::String()).toString();
        if (valueID.isNotEmpty())
            slider.getValueObject().referTo (getMagicState().getPropertyAsValue (valueID));

        auto paramID = getControlledParameterID ({});
        if (paramID.isNotEmpty())
            attachment = getMagicState().createAttachment (paramID, slider);
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> props;

        props.push_back ({ configNode, IDs::parameter, SettableProperty::Choice, {}, magicBuilder.createParameterMenuLambda() });
        props.push_back ({ configNode, pSliderType, SettableProperty::Choice, pSliderTypes [0], magicBuilder.createChoicesMenuLambda (pSliderTypes) });
        props.push_back ({ configNode, pSliderTextBox, SettableProperty::Choice, pTextBoxPositions [2], magicBuilder.createChoicesMenuLambda (pTextBoxPositions) });
        props.push_back ({ configNode, pValue, SettableProperty::Choice, 1.0f, magicBuilder.createPropertiesMenuLambda() });
        props.push_back ({ configNode, pMinValue, SettableProperty::Number, 0.0f, {} });
        props.push_back ({ configNode, pMaxValue, SettableProperty::Number, 2.0f, {} });

        return props;
    }

    juce::String getControlledParameterID (juce::Point<int>) override
    {
        return configNode.getProperty (IDs::parameter, juce::String()).toString();
    }

    juce::Component* getWrappedComponent() override
    {
        return &slider;
    }

private:
    AutoOrientationSlider slider;
    std::unique_ptr<juce::SliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderItem)
};
const juce::Identifier  SliderItem::pSliderType   { "slider-type" };
const juce::StringArray SliderItem::pSliderTypes  { "auto", "linear-horizontal", "linear-vertical", "rotary", "rotary-horizontal-vertical", "inc-dec-buttons" };
const juce::Identifier  SliderItem::pSliderTextBox    { "slider-textbox" };
const juce::StringArray SliderItem::pTextBoxPositions { "no-textbox", "textbox-above", "textbox-below", "textbox-left", "textbox-right" };
const juce::Identifier  SliderItem::pValue      { "value" };
const juce::Identifier  SliderItem::pMinValue   { "min-value" };
const juce::Identifier  SliderItem::pMaxValue   { "max-value" };


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
        attachment.reset();

        auto paramID = configNode.getProperty (IDs::parameter, juce::String()).toString();
        if (paramID.isNotEmpty())
        {
            if (auto* parameter = getMagicState().getParameter (paramID))
            {
                comboBox.clear();
                comboBox.addItemList (parameter->getAllValueStrings(), 1);
                attachment = getMagicState().createAttachment (paramID, comboBox);
            }
        }
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> props;
        props.push_back ({ configNode, IDs::parameter, SettableProperty::Choice, {}, magicBuilder.createParameterMenuLambda() });
        return props;
    }

    juce::Component* getWrappedComponent() override
    {
        return &comboBox;
    }

private:
    juce::ComboBox comboBox;
    std::unique_ptr<juce::ComboBoxParameterAttachment> attachment;

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
        attachment.reset();

        auto parameter = configNode.getProperty (IDs::parameter, juce::String()).toString();
        if (parameter.isNotEmpty())
            attachment = getMagicState().createAttachment (parameter, button);

        button.setClickingTogglesState (parameter.isNotEmpty());
        button.setButtonText (magicBuilder.getStyleProperty (pText, configNode));

        auto triggerID = getProperty (pOnClick).toString();
        if (triggerID.isNotEmpty())
            button.onClick = getMagicState().getTrigger (triggerID);
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> props;

        props.push_back ({ configNode, IDs::parameter, SettableProperty::Choice, {}, magicBuilder.createParameterMenuLambda() });
        props.push_back ({ configNode, pText, SettableProperty::Text, {}, {} });
        props.push_back ({ configNode, pOnClick, SettableProperty::Choice, {}, magicBuilder.createTriggerMenuLambda() });

        return props;
    }

    juce::Component* getWrappedComponent() override
    {
        return &button;
    }

private:
    juce::TextButton button;
    std::unique_ptr<juce::ButtonParameterAttachment> attachment;

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
        attachment.reset();
        auto parameter = configNode.getProperty (IDs::parameter, juce::String()).toString();
        if (parameter.isNotEmpty())
            attachment = getMagicState().createAttachment (parameter, button);

        button.setButtonText (magicBuilder.getStyleProperty (pText, configNode));

        auto propertyID = getProperty (pValue).toString();
        if (propertyID.isNotEmpty())
            button.getToggleStateValue().referTo (getMagicState().getPropertyAsValue (propertyID));
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> props;
        props.push_back ({ configNode, pText, SettableProperty::Text, {}, {} });
        props.push_back ({ configNode, IDs::parameter, SettableProperty::Choice, {}, magicBuilder.createParameterMenuLambda() });
        props.push_back ({ configNode, pValue, SettableProperty::Choice, {}, magicBuilder.createPropertiesMenuLambda() });
        return props;
    }

    juce::Component* getWrappedComponent() override
    {
        return &button;
    }

private:
    juce::ToggleButton button;
    std::unique_ptr<juce::ButtonParameterAttachment> attachment;

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

        auto propertyPath = getProperty (pValue).toString();
        if (propertyPath.isNotEmpty())
            label.getTextValue().referTo (getMagicState().getPropertyAsValue (propertyPath));
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> props;
        props.push_back ({ configNode, pText, SettableProperty::Text, {}, {} });
        props.push_back ({ configNode, pJustification, SettableProperty::Choice, {}, magicBuilder.createChoicesMenuLambda (getAllKeyNames (makeJustificationsChoices())) });
        props.push_back ({ configNode, pFontSize, SettableProperty::Number, {}, {} });
        props.push_back ({ configNode, pEditable, SettableProperty::Toggle, {}, {} });
        props.push_back ({ configNode, pValue, SettableProperty::Choice, {}, magicBuilder.createPropertiesMenuLambda() });
        return props;
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
        auto sourceID = configNode.getProperty (IDs::source, juce::String()).toString();
        if (sourceID.isNotEmpty())
            plot.setPlotSource (getMagicState().getObjectWithType<MagicPlotSource>(sourceID));

        auto decay = float (getProperty (pDecay));
        plot.setDecayFactor (decay);
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> props;
        props.push_back ({ configNode, IDs::source, SettableProperty::Choice, {}, magicBuilder.createObjectsMenuLambda<MagicPlotSource>() });
        props.push_back ({ configNode, pDecay,      SettableProperty::Number, {}, {} });
        return props;
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

    static const juce::Identifier  pCrosshair;
    static const juce::StringArray pCrosshairTypes;
    static const juce::Identifier  pRadius;
    static const juce::Identifier  pSenseFactor;

    XYDraggerItem (MagicGUIBuilder& builder, const juce::ValueTree& node)
      : GuiItem (builder, node)
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
        auto xParamID = configNode.getProperty (IDs::parameterX, juce::String()).toString();
        if (xParamID.isNotEmpty())
            dragger.setParameterX (dynamic_cast<juce::RangedAudioParameter*>(getMagicState().getParameter (xParamID)));
        else
            dragger.setParameterX (nullptr);

        auto yParamID = configNode.getProperty (IDs::parameterY, juce::String()).toString();
        if (yParamID.isNotEmpty())
            dragger.setParameterY (dynamic_cast<juce::RangedAudioParameter*>(getMagicState().getParameter (yParamID)));
        else
            dragger.setParameterY (nullptr);


        auto rightParamID = configNode.getProperty ("right-click", juce::String()).toString();
        if (rightParamID.isNotEmpty())
            dragger.setRightClickParameter (dynamic_cast<juce::RangedAudioParameter*>(getMagicState().getParameter (rightParamID)));

        auto crosshair = getProperty (pCrosshair);
        if (crosshair == pCrosshairTypes [0])
            dragger.setCrossHair (false, false);
        else if (crosshair == pCrosshairTypes [1])
            dragger.setCrossHair (true, false);
        else if (crosshair == pCrosshairTypes [2])
            dragger.setCrossHair (false, true);
        else
            dragger.setCrossHair (true, true);

        auto radius = getProperty (pRadius);
        if (! radius.isVoid())
            dragger.setRadius (radius);

        auto factor = getProperty (pSenseFactor);
        if (! factor.isVoid())
            dragger.setSenseFactor (factor);
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> props;

        props.push_back ({ configNode, IDs::parameterX, SettableProperty::Choice, {}, magicBuilder.createParameterMenuLambda() });
        props.push_back ({ configNode, IDs::parameterY, SettableProperty::Choice, {}, magicBuilder.createParameterMenuLambda() });
        props.push_back ({ configNode, "right-click", SettableProperty::Choice, {}, magicBuilder.createParameterMenuLambda() });
        props.push_back ({ configNode, pCrosshair, SettableProperty::Choice, {}, magicBuilder.createChoicesMenuLambda (pCrosshairTypes) });
        props.push_back ({ configNode, pRadius, SettableProperty::Number, {}, {}});
        props.push_back ({ configNode, pSenseFactor, SettableProperty::Number, {}, {}});

        return props;
    }

    juce::Component* getWrappedComponent() override
    {
        return &dragger;
    }

private:
    XYDragComponent dragger;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYDraggerItem)
};
const juce::Identifier  XYDraggerItem::pCrosshair       { "xy-crosshair" };
const juce::StringArray XYDraggerItem::pCrosshairTypes  { "no-crosshair", "vertical", "horizontal", "crosshair" };
const juce::Identifier  XYDraggerItem::pRadius          { "xy-radius" };
const juce::Identifier  XYDraggerItem::pSenseFactor     { "xy-sense-factor" };

//==============================================================================

class KeyboardItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (KeyboardItem)

    KeyboardItem (MagicGUIBuilder& builder, const juce::ValueTree& node)
      : GuiItem (builder, node),
        keyboard (getMagicState().getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
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
        auto sourceID = configNode.getProperty (IDs::source, juce::String()).toString();
        if (sourceID.isNotEmpty())
            meter.setLevelSource (getMagicState().getObjectWithType<MagicLevelSource>(sourceID));
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> props;
        props.push_back ({ configNode, IDs::source, SettableProperty::Choice, {}, magicBuilder.createObjectsMenuLambda<MagicLevelSource>() });
        return props;
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

class MidiLearnItem : public GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (MidiLearnItem)

    MidiLearnItem (MagicGUIBuilder& builder, const juce::ValueTree& node) : GuiItem (builder, node)
    {
        if (auto* state = dynamic_cast<MagicProcessorState*>(&builder.getMagicState()))
            midiLearn.setMagicProcessorState (state);

        addAndMakeVisible (midiLearn);
    }

    void update() override {}

    juce::Component* getWrappedComponent() override
    {
        return &midiLearn;
    }

private:
    MidiLearnComponent midiLearn;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiLearnItem)
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

    ~ListBoxItem() override
    {
        if (auto* m = dynamic_cast<juce::ChangeBroadcaster*>(listBox.getModel()))
            m->removeChangeListener (this);
    }

    void update() override
    {
        if (auto* m = dynamic_cast<juce::ChangeBroadcaster*>(listBox.getModel()))
            m->removeChangeListener (this);

        auto modelID = configNode.getProperty ("list-box-model", juce::String()).toString();
        if (modelID.isNotEmpty())
        {
            if (auto* model = getMagicState().getObjectWithType<juce::ListBoxModel>(modelID))
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

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> props;
        props.push_back ({ configNode, "list-box-model", SettableProperty::Choice, {}, magicBuilder.createObjectsMenuLambda<juce::ListBoxModel>() });
        return props;
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
    registerFactory ("MidiLearn", &MidiLearnItem::factory);
    registerFactory (IDs::listBox, &ListBoxItem::factory);

#if JUCE_MODULE_AVAILABLE_juce_gui_extra && JUCE_WEB_BROWSER
    registerFactory (IDs::webBrowser, &WebBrowserItem::factory);
#endif // JUCE_WEB_BROWSER
}

} // namespace foleys
