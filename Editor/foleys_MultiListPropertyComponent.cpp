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


namespace foleys
{

MultiListPropertyComponent::MultiListPropertyComponent (const juce::Value& valueToControl, const juce::String& propertyName, const juce::StringArray &choicesToUse, const juce::String& separatorToUse, int preferredHeight)
  : juce::PropertyComponent (propertyName, preferredHeight),
    choices (choicesToUse),
    separator (separatorToUse)
{
    text.setEditable (true);
    text.getTextValue().referTo (valueToControl);
    text.onTextChange = [&]
    {
        auto strings = juce::StringArray::fromTokens (text.getText(), separator, "");
        strings.removeEmptyStrings (true);
        text.setText (strings.joinIntoString (separator), juce::sendNotificationAsync);
    };

    addAndMakeVisible (text);
    addAndMakeVisible (select);

    select.onClick = [&]
    {
        auto strings = juce::StringArray::fromTokens (text.getText(), separator, "");

        juce::PopupMenu popup;
        for (const auto& name : choices)
            if (! strings.contains (name))
                popup.addItem (name, [&]
                {
                    if (! strings.contains (name))
                    {
                        strings.add (name);
                        strings.removeEmptyStrings (true);
                        text.setText (strings.joinIntoString (separator), juce::sendNotificationAsync);
                    }
                });

        popup.showAt (getScreenBounds());
    };
}

void MultiListPropertyComponent::refresh()
{
}

void MultiListPropertyComponent::resized()
{
    auto area = getLookAndFeel().getPropertyComponentContentPosition (*this);

    select.setBounds (area.removeFromRight (getHeight()));
    text.setBounds (area);
}


} // namespace foleys
