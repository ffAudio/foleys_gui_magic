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


namespace foleys
{


RadioButtonHandler::RadioButtonHandler (juce::Button& buttonToControl, RadioButtonManager& manager)
  : button (buttonToControl),
    radioButtonManager (manager)
{
    radioButtonManager.addButton (&button);
    button.addListener (this);
}

RadioButtonHandler::~RadioButtonHandler()
{
    if (parameter)
        parameter->removeListener (this);

    button.removeListener (this);
    radioButtonManager.removeButton (&button);
}

void RadioButtonHandler::setRadioGroupValue (juce::var value, juce::RangedAudioParameter* parameterToControl)
{
    if (parameterToControl != parameter)
    {
        if (parameter)
            parameter->removeListener (this);

        parameter = parameterToControl;
        if (parameter)
            parameter->addListener (this);
    }

    radioButtonValue = value;

    if (parameter)
    {
        auto currentValue = parameter->convertFrom0to1 (parameter->getValue());
        // other than setToggleState this seems not to trigger circular updates
        button.getToggleStateValue() = (currentValue == static_cast<float>(value));
    }
}

void RadioButtonHandler::buttonClicked (juce::Button* clickedButton)
{
    if (parameter)
    {
        auto value = parameter->convertTo0to1 (radioButtonValue);
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost (value);
        parameter->endChangeGesture();
    }

    radioButtonManager.buttonActivated (clickedButton);
}

void RadioButtonHandler::parameterValueChanged (int parameterIndex, float newValue)
{
    juce::ignoreUnused (parameterIndex);

    if (!parameter)
        return;

    auto value = parameter->convertFrom0to1 (newValue);
    // other than setToggleState this seems not to trigger circular updates
    button.getToggleStateValue() = (value == static_cast<float>(radioButtonValue));
}

// ==============================================================================

void RadioButtonManager::buttonActivated (juce::Button* button)
{
    auto groupID = button->getRadioGroupId();
    if (groupID == 0)
        return;

    for (auto& otherButton : buttons)
        if (otherButton && button != otherButton && otherButton->getRadioGroupId() == groupID)
            otherButton->getToggleStateValue() = false;
}

void RadioButtonManager::addButton (juce::Button* button)
{
    if (std::find(buttons.begin(), buttons.end(), button) == buttons.end())
        buttons.push_back(button);
}

void RadioButtonManager::removeButton (juce::Button* button)
{
    buttons.erase (std::remove_if (buttons.begin(), buttons.end(), [button](const auto& other)
                        { return other == button || other == nullptr; }),
                   buttons.end());
}


} // namespace foleys
