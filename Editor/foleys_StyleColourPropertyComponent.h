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

class StyleColourPropertyComponent  : public StylePropertyComponent,
                                      private juce::Value::Listener,
                                      private juce::ChangeListener
{
public:
    StyleColourPropertyComponent (MagicGUIBuilder& builderToUse, juce::Identifier propertyToUse, juce::ValueTree& nodeToUse);
    ~StyleColourPropertyComponent();

    void refresh() override;

private:

    class ColourPanel : public juce::Component
    {
    public:
        ColourPanel (juce::Colour colour);
        void resized() override;

        void addChangeListener (juce::ChangeListener*);
        void removeChangeListener (juce::ChangeListener*);

    private:

        class ColourSelectorWithSwatches  : public juce::ColourSelector
        {
        public:
            ColourSelectorWithSwatches();
            ~ColourSelectorWithSwatches();

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

        juce::TextButton            close {"X"};
        ColourSelectorWithSwatches  selector;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourPanel)
    };

    void valueChanged (juce::Value& value) override;

    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    void setColourDisplay (juce::Colour colour);

    void getLookAndFeelColourFallback();

    MouseLambdas mouseEvents;
    juce::Component::SafePointer<ColourPanel> colourPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StyleColourPropertyComponent)
};


} // namespace foleys
