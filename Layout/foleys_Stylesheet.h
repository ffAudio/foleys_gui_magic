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

class Stylesheet
{
public:

    Stylesheet() = default;

    void setStyle (const juce::ValueTree& node);

    /**
     This method traverses the dom and checks each style, if that property was defined.

     @param name the name of the property.
     @param node is the node in the DOM. This is used for inheritance by traversing upwards.
     */
    juce::var getProperty (const juce::Identifier& name, const juce::ValueTree& node, bool inherit=true) const;

    juce::LookAndFeel* getLookAndFeel (const juce::ValueTree& node) const;

    juce::Image getBackgroundImage (const juce::ValueTree& node) const;

    juce::Array<juce::Colour> getBackgroundGradient (const juce::ValueTree& node) const;

    juce::Colour parseColour (const juce::String& name) const;

    juce::ValueTree getCurrentStyle() const;

    /**
     With that method you can register your custom LookAndFeel class and apply it to different components.
     */
    void registerLookAndFeel (juce::String name, std::unique_ptr<juce::LookAndFeel> lookAndFeel);

    void configureFlexBox (juce::FlexBox& flexBox, const juce::ValueTree& node) const;

    void configureFlexBoxItem (juce::FlexItem& item, const juce::ValueTree& node) const;

    static juce::ValueTree createDefaultStyle();


private:

    juce::StringArray getParameters (const juce::String& text) const;

    juce::ValueTree   currentStyle;

    std::map<juce::String, std::unique_ptr<juce::LookAndFeel>> lookAndFeels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Stylesheet)
};

} // namespace foleys
