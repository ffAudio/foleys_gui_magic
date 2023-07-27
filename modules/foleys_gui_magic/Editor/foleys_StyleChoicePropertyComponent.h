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

class StyleChoicePropertyComponent  : public StylePropertyComponent,
                                      private juce::Value::Listener
{
public:
    StyleChoicePropertyComponent (MagicGUIBuilder& builderToUse, juce::Identifier propertyToUse, juce::ValueTree& nodeToUse, juce::StringArray choices);
    StyleChoicePropertyComponent (MagicGUIBuilder& builderToUse, juce::Identifier propertyToUse, juce::ValueTree& nodeToUse, std::function<void(juce::ComboBox&)> menuCreationLambda);

    void refresh() override;

private:
    void initialiseComboBox (bool editable);

    void valueChanged (juce::Value& value) override;

    juce::StringArray               choices;
    std::function<void(juce::ComboBox&)> menuCreationLambda;
    juce::Value                     proxy;

    bool                            updating = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StyleChoicePropertyComponent)
};


} // namespace foleys
