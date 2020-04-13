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


void MagicGUIBuilder::registerJUCEFactories()
{
    registerFactory (IDs::slider,
                     [] (const juce::ValueTree&)
                     {
                         return std::make_unique<AttachableSlider>();
                     });

    setColourTranslation (IDs::slider,
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

    addSettableProperty (IDs::slider,
                         std::make_unique<SettableChoiceProperty>
                         (IDs::parameter,
                          [&] (juce::Component* component, juce::var value, const juce::NamedValueSet&)
                          {
                              if (magicState)
                                  if (auto* slider = dynamic_cast<AttachableSlider*>(component))
                                      slider->attachToParameter (value.toString(), magicState->getValueTreeState());
                          },
                          juce::var(),
                          SettableProperty::Parameter));

    addSettableProperty (IDs::slider,
                         std::make_unique<SettableChoiceProperty>
                         (
                          "slider-type",
                          [] (juce::Component* component, juce::var value, const juce::NamedValueSet& options)
                          {
                              if (auto* slider = dynamic_cast<AttachableSlider*>(component))
                              {
                                  const auto name = value.toString();
                                  if (name.isEmpty())
                                      return;

                                  if (name == "auto")
                                      slider->setAutoOrientation (true);
                                  else
                                  {
                                      slider->setAutoOrientation (false);
                                      const auto& v = options [name];
                                      if (v.isVoid() == false)
                                          slider->setSliderStyle (juce::Slider::SliderStyle ( int (v)));
                                  }
                              }
                          },
                          "auto",
                          juce::NamedValueSet {
                              { "linear-horizontal", juce::Slider::LinearHorizontal },
                              { "linear-vertical",   juce::Slider::LinearVertical },
                              { "rotary",            juce::Slider::RotaryHorizontalVerticalDrag },
                              { "inc-dec-buttons",   juce::Slider::IncDecButtons },
                              { "auto",              0 }
                          }));

    addSettableProperty (IDs::slider,
                         std::make_unique<SettableChoiceProperty>
                         (
                          "slider-textbox",
                          [] (juce::Component* component, juce::var value, const juce::NamedValueSet& options)
                          {
                              if (auto* slider = dynamic_cast<juce::Slider*>(component))
                              {
                                  const auto& v = options [value.toString()];
                                  if (v.isVoid() == false)
                                      slider->setTextBoxStyle (juce::Slider::TextEntryBoxPosition (int (v)),
                                                               false, slider->getTextBoxWidth(), slider->getTextBoxHeight());
                              }
                          },
                          "textbox-below",
                          juce::NamedValueSet {
                              { "no-textbox",    juce::Slider::NoTextBox },
                              { "textbox-left",  juce::Slider::TextBoxLeft },
                              { "textbox-right", juce::Slider::TextBoxRight },
                              { "textbox-above", juce::Slider::TextBoxAbove },
                              { "textbox-below", juce::Slider::TextBoxBelow }
                          }));

    //==============================================================================

    registerFactory (IDs::comboBox,
                     [] (const juce::ValueTree&)
                     {
                         return std::make_unique<AttachableComboBox>();
                     });

    setColourTranslation (IDs::comboBox,
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

    addSettableProperty (IDs::comboBox,
                         std::make_unique<SettableChoiceProperty>
                         (IDs::parameter,
                          [&] (juce::Component* component, juce::var value, const juce::NamedValueSet&)
                          {
                              if (magicState)
                                  if (auto* combo = dynamic_cast<AttachableComboBox*>(component))
                                      combo->attachToParameter (value.toString(), magicState->getValueTreeState());
                          },
                          juce::String(),
                          SettableProperty::Parameter));

    //==============================================================================

    registerFactory (IDs::textButton,
                     [] (const juce::ValueTree&)
                     {
                         return std::make_unique<AttachableTextButton>();
                     });

    setColourTranslation (IDs::textButton,
                          {
                              { "button-color", juce::TextButton::buttonColourId },
                              { "button-on-color", juce::TextButton::buttonOnColourId },
                              { "button-off-text", juce::TextButton::textColourOffId },
                              { "button-on-text", juce::TextButton::textColourOnId }
                          });

    addSettableProperty (IDs::textButton,
                         std::make_unique<SettableTextProperty>
                         (
                          "text",
                          juce::var(),
                          [] (juce::Component* component, juce::var value)
                          {
                              if (auto* button = dynamic_cast<juce::TextButton*>(component))
                                  button->setButtonText (value.toString());
                          }));

    addSettableProperty (IDs::textButton,
                         std::make_unique<SettableChoiceProperty>
                         (IDs::parameter,
                          [&] (juce::Component* component, juce::var value, const juce::NamedValueSet&)
                          {
                              if (magicState)
                                  if (auto* button = dynamic_cast<AttachableTextButton*>(component))
                                      button->attachToParameter (value.toString(), magicState->getValueTreeState());
                          },
                          juce::String(),
                          SettableProperty::Parameter));

    addSettableProperty (IDs::textButton,
                         std::make_unique<SettableChoiceProperty>
                         ("onClick",
                          [&] (juce::Component* component, juce::var value, const juce::NamedValueSet&)
                          {
                              if (magicState)
                                  if (auto* button = dynamic_cast<AttachableTextButton*>(component))
                                      button->onClick = magicState->getTrigger (value.toString());
                          },
                          juce::String(),
                          SettableProperty::Trigger));

    //==============================================================================

    registerFactory (IDs::toggleButton,
                     [] (const juce::ValueTree&)
                     {
                         return std::make_unique<AttachableToggleButton>();
                     });

    setColourTranslation (IDs::toggleButton,
                          {
                              { "toggle-text", juce::ToggleButton::textColourId },
                              { "toggle-tick", juce::ToggleButton::tickColourId },
                              { "toggle-tick-disabled", juce::ToggleButton::tickDisabledColourId }
                          });

    addSettableProperty (IDs::toggleButton,
                         std::make_unique<SettableTextProperty>
                         (
                          "text",
                          juce::var(),
                          [] (juce::Component* component, juce::var value)
                          {
                              if (auto* button = dynamic_cast<juce::ToggleButton*>(component))
                                  button->setButtonText (value.toString());
                          }));

    addSettableProperty (IDs::toggleButton,
                         std::make_unique<SettableChoiceProperty>
                         ("value",
                          [&] (juce::Component* component, juce::var value, const juce::NamedValueSet&)
                          {
                              if (value.toString().isEmpty())
                                  return;

                              if (magicState)
                              {
                                  if (auto* button = dynamic_cast<AttachableToggleButton*>(component))
                                  {
                                      button->setClickingTogglesState (true);
                                      button->getToggleStateValue().referTo (magicState->getPropertyAsValue (value.toString()));
                                  }
                              }
                          },
                          juce::String(),
                          SettableProperty::Property));

    addSettableProperty (IDs::toggleButton,
                         std::make_unique<SettableChoiceProperty>
                         (IDs::parameter,
                          [&] (juce::Component* component, juce::var value, const juce::NamedValueSet&)
                          {
                              if (magicState)
                                  if (auto* button = dynamic_cast<AttachableToggleButton*>(component))
                                      button->attachToParameter (value.toString(), magicState->getValueTreeState());
                          },
                          juce::String(),
                          SettableProperty::Parameter));

    //==============================================================================

    registerFactory (IDs::label,
                     [] (const juce::ValueTree&)
                     {
                         return std::make_unique<juce::Label>();
                     });

    setColourTranslation (IDs::label,
                          {
                              { "label-background",         juce::Label::backgroundColourId },
                              { "label-outline",            juce::Label::outlineColourId },
                              { "label-text",               juce::Label::textColourId },
                              { "label-editing-background", juce::Label::backgroundWhenEditingColourId },
                              { "label-editing-outline",    juce::Label::outlineWhenEditingColourId },
                              { "label-editing-text",       juce::Label::textWhenEditingColourId }
                          });

    addSettableProperty (IDs::label,
                         std::make_unique<SettableTextProperty>
                         (
                          "text",
                          juce::var(),
                          [] (juce::Component* component, juce::var value)
                          {
                              if (auto* label = dynamic_cast<juce::Label*>(component))
                                  label->setText (value.toString(), juce::dontSendNotification);
                          }));

    addSettableProperty (IDs::label,
                         std::make_unique<SettableValueProperty>
                         (
                          "value",
                          juce::var(),
                          [&] (juce::Component* component, juce::var value)
                          {
                              if (magicState == nullptr)
                                  return;

                              if (auto* label = dynamic_cast<juce::Label*>(component))
                                  label->getTextValue().referTo (magicState->getPropertyAsValue (value.toString()));
                          }));

    addSettableProperty (IDs::label,
                         std::make_unique<SettableChoiceProperty>
                         (
                          "justification",
                          [] (juce::Component* component, juce::var value, const juce::NamedValueSet& options)
                          {
                              if (auto* label = dynamic_cast<juce::Label*>(component))
                              {
                                  const auto flags = int (options [value.toString()]);
                                  label->setJustificationType (flags);
                              }
                          },
                          "centred-left",
                          makeJustificationsChoices()));

    addSettableProperty (IDs::label,
                         std::make_unique<SettableNumberProperty>
                         (
                          "font-size",
                          0,
                          [] (juce::Component* component, juce::var value)
                          {
                              if (auto* label = dynamic_cast<juce::Label*>(component))
                              {
                                  if (value.isVoid() == false)
                                      label->setFont (juce::Font (float (value)));
                              }
                          }));

    addSettableProperty (IDs::label,
                         std::make_unique<SettableBoolProperty>
                         (
                          "editable",
                          juce::var(),
                          [] (juce::Component* component, juce::var value)
                          {
                              if (auto* label = dynamic_cast<juce::Label*>(component))
                                  label->setEditable (value);
                          }));

    //==============================================================================

#if JUCE_MODULE_AVAILABLE_juce_gui_extra && JUCE_WEB_BROWSER
    registerFactory (IDs::webBrowser,
                     [] (const juce::ValueTree&)
                     {
                         return std::make_unique<juce::WebBrowserComponent>();
                     });

    addSettableProperty (IDs::webBrowser,
                         std::make_unique<SettableTextProperty>
                         (
                          "url",
                          juce::var(),
                          [] (juce::Component* component, juce::var value)
                          {
                              if (auto* browser = dynamic_cast<juce::WebBrowserComponent*>(component))
                                  browser->goToURL (value.toString());
                          }));
#endif

    //==============================================================================

    registerFactory (IDs::plot,
                     [] (const juce::ValueTree&)
                     {
                         return std::make_unique<MagicPlotComponent>();
                     });

    setColourTranslation (IDs::plot,
                          {
                              { "plot-color", MagicPlotComponent::plotColourId },
                              { "plot-fill-color", MagicPlotComponent::plotFillColourId },
                              { "plot-inactive-color", MagicPlotComponent::plotInactiveColourId },
                              { "plot-inactive-fill-color", MagicPlotComponent::plotInactiveFillColourId }
                          });

    addSettableProperty (IDs::plot,
                         std::make_unique<SettableChoiceProperty>
                         (IDs::source,
                          [&] (juce::Component* component, juce::var value, const juce::NamedValueSet&)
                          {
                              if (magicState)
                                  if (auto* plotComponent = dynamic_cast<MagicPlotComponent*>(component))
                                      plotComponent->setPlotSource (magicState->getPlotSource (value.toString()));
                          },
                          juce::String(),
                          SettableProperty::PlotSource));

    addSettableProperty (IDs::plot,
                         std::make_unique<SettableNumberProperty>
                         ("plot-decay",
                          0.0f,
                          [] (juce::Component* component, juce::var value)
                          {
                              if (auto* plotComponent = dynamic_cast<MagicPlotComponent*>(component))
                                  plotComponent->setDecayFactor (float (value));
                          }));

    //==============================================================================

    registerFactory (IDs::xyDragComponent,
                     [&] (const juce::ValueTree&)
                     {
                         return (magicState ? std::make_unique<XYDragComponent>(magicState->getValueTreeState())
                                            : std::unique_ptr<XYDragComponent>());
                     });

    setColourTranslation (IDs::xyDragComponent,
                          {
                              { "xy-drag-handle",      XYDragComponent::xyDotColourId },
                              { "xy-drag-handle-over", XYDragComponent::xyDotOverColourId },
                              { "xy-horizontal",       XYDragComponent::xyHorizontalColourId },
                              { "xy-horizontal-over",  XYDragComponent::xyHorizontalOverColourId },
                              { "xy-vertical",         XYDragComponent::xyVerticalColourId },
                              { "xy-vertical-over",    XYDragComponent::xyVerticalOverColourId }
                          });

    addSettableProperty (IDs::xyDragComponent,
                         std::make_unique<SettableChoiceProperty>
                         (IDs::parameterX,
                          [] (juce::Component* component, juce::var value, const juce::NamedValueSet&)
                          {
                              if (auto* xydrag = dynamic_cast<XYDragComponent*>(component))
                                  xydrag->setParameterX (value.toString());
                          },
                          juce::String(),
                          SettableProperty::Parameter));

    addSettableProperty (IDs::xyDragComponent,
                         std::make_unique<SettableChoiceProperty>
                         (IDs::parameterY,
                          [] (juce::Component* component, juce::var value, const juce::NamedValueSet&)
                          {
                              if (auto* xydrag = dynamic_cast<XYDragComponent*>(component))
                                  xydrag->setParameterY (value.toString());
                          },
                          juce::String(),
                          SettableProperty::Parameter));

    addSettableProperty (IDs::xyDragComponent,
                         std::make_unique<SettableChoiceProperty>
                         (
                          "xy-crosshair",
                          [] (juce::Component* component, juce::var value, const auto& options)
                          {
                              if (auto* xydrag = dynamic_cast<XYDragComponent*>(component))
                              {
                                  const auto name = value.toString();
                                  if (name.isEmpty())
                                      return;

                                  const auto& v = options [name];
                                  if (v.isVoid() == false)
                                  {
                                      int bits = v;
                                      xydrag->setCrossHair (bits & 0x01, bits & 0x02);
                                  }
                              }
                          },
                          "crosshair",
                          juce::NamedValueSet {
                              { "no-crosshair", 0x00 },
                              { "vertical",     0x01 },
                              { "horizontal",   0x02 },
                              { "crosshair",    0x03 }
                          }));

    //==============================================================================

    registerFactory (IDs::keyboardComponent,
                     [&] (const juce::ValueTree&)
                     {
                         if (magicState)
                         {
                             auto item = std::make_unique<juce::MidiKeyboardComponent>(magicState->getKeyboardState(),
                                                                                       juce::MidiKeyboardComponent::horizontalKeyboard);
                             item->setKeyWidth (50.0f);

                             return item;
                         }

                         return std::unique_ptr<juce::MidiKeyboardComponent>();
                     });

    setColourTranslation (IDs::keyboardComponent,
                          {
                              { "white-note-color",      juce::MidiKeyboardComponent::whiteNoteColourId },
                              { "black-note-color",      juce::MidiKeyboardComponent::blackNoteColourId },
                              { "key-separator-line-color", juce::MidiKeyboardComponent::keySeparatorLineColourId },
                              { "mouse-over-color",      juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId },
                              { "key-down-color",        juce::MidiKeyboardComponent::keyDownOverlayColourId },
                          });

    addSettableProperty (IDs::keyboardComponent,
                         std::make_unique<SettableChoiceProperty>
                         (
                          "orientation",
                          [] (juce::Component* component, juce::var value, const juce::NamedValueSet& options)
                          {
                              if (auto* keyboard = dynamic_cast<juce::MidiKeyboardComponent*>(component))
                              {
                                  const auto& v = options [value.toString()];
                                  if (v.isVoid() == false)
                                      keyboard->setOrientation (juce::MidiKeyboardComponent::Orientation (int (v)));
                              }
                          },
                          "horizontal",
                          juce::NamedValueSet {
                              { "horizontal",     juce::MidiKeyboardComponent::horizontalKeyboard },
                              { "vertical-left",  juce::MidiKeyboardComponent::verticalKeyboardFacingLeft },
                              { "vertical-right", juce::MidiKeyboardComponent::verticalKeyboardFacingRight }
                          }));

    addSettableProperty (IDs::keyboardComponent,
                         std::make_unique<SettableNumberProperty>
                         (
                          "key-width",
                          50,
                          [] (juce::Component* component, juce::var value)
                          {
                              if (auto* keyboard = dynamic_cast<juce::MidiKeyboardComponent*>(component))
                              {
                                  if (value.isVoid() == false)
                                      keyboard->setKeyWidth (float (value));
                              }
                          }));

    //==============================================================================

    registerFactory (IDs::meter,
                     [] (const juce::ValueTree&)
                     {
                         return std::make_unique<MagicLevelMeter>();
                     });

    setColourTranslation (IDs::meter,
                          {
                              { "background-color", MagicLevelMeter::backgroundColourId },
                              { "bar-background-color", MagicLevelMeter::barBackgroundColourId },
                              { "outline-color", MagicLevelMeter::outlineColourId },
                              { "bar-fill-color", MagicLevelMeter::barFillColourId },
                              { "tickmark-color", MagicLevelMeter::tickmarkColourId },
                          });

    addSettableProperty (IDs::meter,
                         std::make_unique<SettableChoiceProperty>
                         (IDs::source,
                          [&] (juce::Component* component, juce::var value, const juce::NamedValueSet&)
                          {
                              if (magicState)
                                  if (auto* meter = dynamic_cast<MagicLevelMeter*>(component))
                                      meter->setLevelSource (magicState->getLevelSource (value.toString()));
                          },
                          juce::String(),
                          SettableProperty::LevelSource));

}

} // namespace foleys
