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


template <typename AppType>
void MagicGUIBuilder<AppType>::registerJUCEFactories()
{
    registerFactory (IDs::slider,
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         return std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
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
    {
        "slider-type",
        {
            { "linear-horizontal", juce::Slider::LinearHorizontal },
            { "linear-vertical",   juce::Slider::LinearVertical },
            { "rotary",            juce::Slider::RotaryHorizontalVerticalDrag },
            { "inc-dec-buttons",   juce::Slider::IncDecButtons }
        },
        [] (juce::Component* component, juce::var value, const SettableProperty::OptionsVector& options)
        {
            if (auto* slider = dynamic_cast<juce::Slider*>(component))
            {
                for (auto& o : options)
                {
                    if (value == o.first)
                    {
                        slider->setSliderStyle (juce::Slider::SliderStyle ( int(o.second)));
                        break;
                    }
                }
            }
        },
        "rotary"
    });

    addSettableProperty (IDs::slider,
                         {
                             "slider-textbox",
                             {
                                 { "no-textbox",    juce::Slider::NoTextBox },
                                 { "textbox-left",  juce::Slider::TextBoxLeft },
                                 { "textbox-right", juce::Slider::TextBoxRight },
                                 { "textbox-above", juce::Slider::TextBoxAbove },
                                 { "textbox-below", juce::Slider::TextBoxBelow }
                             },
                             [] (juce::Component* component, juce::var value, const SettableProperty::OptionsVector& options)
                             {
                                 if (auto* slider = dynamic_cast<juce::Slider*>(component))
                                 {
                                     for (auto& o : options)
                                     {
                                         if (value == o.first)
                                         {
                                             slider->setTextBoxStyle (juce::Slider::TextEntryBoxPosition (int (o.second)),
                                                                      false, slider->getTextBoxWidth(), slider->getTextBoxHeight());
                                             break;
                                         }
                                     }
                                 }
                             },
                             "textbox-below"
                         });

    //==============================================================================

    registerFactory (IDs::comboBox,
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         return std::make_unique<juce::ComboBox>();
                     });

    setColourTranslation (IDs::comboBox,
                          {
                              { "combo-background", juce::ComboBox::backgroundColourId },
                              { "combo-text", juce::ComboBox::textColourId },
                              { "combo-outline", juce::ComboBox::outlineColourId },
                              { "combo-button", juce::ComboBox::buttonColourId },
                              { "combo-arrow", juce::ComboBox::arrowColourId },
                              { "combo-focused-outline", juce::ComboBox::focusedOutlineColourId }
                          });

    //==============================================================================

    registerFactory (IDs::textButton,
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         return std::make_unique<juce::TextButton>();
                     });

    setColourTranslation (IDs::textButton,
                          {
                              { "button-color", juce::TextButton::buttonColourId },
                              { "button-on-color", juce::TextButton::buttonOnColourId },
                              { "button-off-text", juce::TextButton::textColourOffId },
                              { "button-on-text", juce::TextButton::textColourOnId }
                          });

    //==============================================================================

    registerFactory (IDs::toggleButton,
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         auto text = config.getProperty (IDs::caption, "Active");
                         return std::make_unique<juce::ToggleButton>(text);
                     });

    setColourTranslation (IDs::toggleButton,
                          {
                              { "toggle-text", juce::ToggleButton::textColourId },
                              { "toggle-tick", juce::ToggleButton::tickColourId },
                              { "toggle-tick-disabled", juce::ToggleButton::tickDisabledColourId }
                          });

    //==============================================================================

    registerFactory (IDs::plot,
                     [&] (const juce::ValueTree& config, auto& app)
                     {
                         auto item = std::make_unique<MagicPlotComponent>();
                         if (magicState && config.hasProperty (IDs::source))
                         {
                             auto sourceID = config.getProperty (IDs::source).toString();
                             if (sourceID.isNotEmpty())
                             {
                                 auto* source = magicState->getPlotSource (sourceID);
                                 item->setPlotSource (source);
                             }
                         }
                         return std::move (item);
                     });

    setColourTranslation (IDs::plot,
                          {
                              { "plot-color", MagicPlotComponent::plotColourId },
                              { "plot-fill-color", MagicPlotComponent::plotFillColourId },
                              { "plot-inactive-color", MagicPlotComponent::plotInactiveColourId },
                              { "plot-inactive-fill-color", MagicPlotComponent::plotInactiveFillColourId }
                          });

}

} // namespace foleys
