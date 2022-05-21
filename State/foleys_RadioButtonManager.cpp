/*
 ==============================================================================
    Copyright (c) 2021-2022 Foleys Finest Audio - Daniel Walz
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


namespace foleys
{

void RadioButtonManager::addButton (juce::Button* button)
{
    if (std::find(buttons.begin(), buttons.end(), button) == buttons.end())
        buttons.push_back(button);

    button->addListener (this);
}

void RadioButtonManager::removeButton (juce::Button* button)
{
    buttons.erase (std::remove_if (buttons.begin(), buttons.end(), [button](const auto& other)
                        { return other == button; }),
                   buttons.end());
    button->removeListener (this);
}

void RadioButtonManager::buttonStateChanged (juce::Button* button)
{
    if (!button->getToggleState())
        return;

    auto groupID = button->getRadioGroupId();
    if (groupID == 0)
        return;

    for (auto* otherButton : buttons)
        if (button != otherButton && otherButton->getRadioGroupId() == groupID)
            otherButton->setToggleState (false, juce::sendNotificationSync);
}

} // namespace foleys
