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

#include "foleys_StylePropertyComponent.h"

namespace foleys
{

class StyleGradientPropertyComponent  : public StylePropertyComponent,
                                        private juce::Value::Listener,
                                        private juce::ChangeListener
{
public:
    StyleGradientPropertyComponent (MagicGUIBuilder& builderToUse, juce::Identifier propertyToUse, juce::ValueTree& nodeToUse);
    ~StyleGradientPropertyComponent() override;

    void refresh() override;
    void resized() override;

private:

    class GradientPanel : public juce::Component
    {
    public:
        GradientPanel (GradientBackground& gradient);
        void resized() override;
        void paint (juce::Graphics& g) override;

        void addChangeListener (juce::ChangeListener*);
        void removeChangeListener (juce::ChangeListener*);

        void colourWasChanged();

    private:

        class ColourSelectorWithSwatches  : public juce::ColourSelector
        {
        public:
            ColourSelectorWithSwatches();
            ~ColourSelectorWithSwatches() override;

            int getNumSwatches() const override;
            juce::Colour getSwatchColour (int index) const override;
            void setSwatchColour (int index, const juce::Colour& colour) override;

        private:
            void loadSwatches();
            void saveSwatches();

            juce::ApplicationProperties properties;

            std::vector<juce::Colour> swatchColours;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourSelectorWithSwatches)
        };

        class GradientStopSelect : public juce::Component
        {
        public:
            GradientStopSelect (GradientPanel& owner);

            void paint (juce::Graphics& g) override;
            void mouseDown (const juce::MouseEvent& event) override;
            void mouseMove (const juce::MouseEvent& event) override;
            void mouseDrag (const juce::MouseEvent& event) override;
            void mouseUp (const juce::MouseEvent& event) override;
            void mouseDoubleClick (const juce::MouseEvent& event) override;

            int  selected =  0;

        private:
            int getDraggingIndex (int pos);

            GradientPanel& owner;
            int            dragging = -1;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GradientStopSelect)
        };

        GradientBackground&         gradient;

        juce::TextButton            close { "X" };
        juce::ComboBox              typeSelect;
        juce::Slider                angle { juce::Slider::LinearHorizontal, juce::Slider::TextBoxLeft };
        ColourSelectorWithSwatches  selector;
        GradientStopSelect          stopSelect;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GradientPanel)
    };

    void valueChanged (juce::Value& value) override;

    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    MouseLambdas                                mouseEvents;
    juce::Component::SafePointer<GradientPanel> colourPanel;
    GradientBackground                          gradient;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StyleGradientPropertyComponent)
};


} // namespace foleys
