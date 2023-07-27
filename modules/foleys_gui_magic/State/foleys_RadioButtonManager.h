/*
 ==============================================================================
    Copyright (c) 2021-2023 Foleys Finest Audio - Daniel Walz
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

namespace foleys
{


class RadioButtonManager
{
public:
    RadioButtonManager() = default;

    void addButton (juce::Button* button);
    void removeButton (juce::Button* button);

    void buttonActivated (juce::Button* button);
private:
    std::vector<juce::Component::SafePointer<juce::Button>> buttons;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RadioButtonManager)

};

// ==============================================================================

class RadioButtonHandler  : private juce::Button::Listener,
                            private juce::RangedAudioParameter::Listener
{
public:
    RadioButtonHandler (juce::Button& buttonToControl, RadioButtonManager& manager);
    ~RadioButtonHandler() override;

    void setRadioGroupValue (juce::var value, juce::RangedAudioParameter* parameter);

private:

    void buttonClicked (juce::Button *) override;
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int /*parameterIndex*/, bool /*gestureIsStarting*/) override {}

    juce::Button& button;
    RadioButtonManager& radioButtonManager;

    juce::var radioButtonValue;
    juce::RangedAudioParameter* parameter = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RadioButtonHandler)
};


} // namespace foleys
