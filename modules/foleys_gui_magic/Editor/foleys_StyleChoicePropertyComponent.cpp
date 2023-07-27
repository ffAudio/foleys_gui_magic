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

#include "foleys_StyleChoicePropertyComponent.h"

namespace foleys
{

StyleChoicePropertyComponent::StyleChoicePropertyComponent (MagicGUIBuilder& builderToUse,
                                                            juce::Identifier propertyToUse,
                                                            juce::ValueTree& nodeToUse,
                                                            juce::StringArray choicesToUse)
  : StylePropertyComponent (builderToUse, propertyToUse, nodeToUse),
    choices (choicesToUse)
{
    initialiseComboBox (false);
}

StyleChoicePropertyComponent::StyleChoicePropertyComponent (MagicGUIBuilder& builderToUse, juce::Identifier propertyToUse, juce::ValueTree& nodeToUse, std::function<void(juce::ComboBox&)> lambdaToUse)
  : StylePropertyComponent (builderToUse, propertyToUse, nodeToUse),
    menuCreationLambda (lambdaToUse)
{
    initialiseComboBox (false);
}

void StyleChoicePropertyComponent::initialiseComboBox (bool editable)
{
    auto combo = std::make_unique<juce::ComboBox>();
    combo->setEditableText (editable);

    if (! choices.isEmpty())
    {
        int index = 0;
        for (const auto& name : choices)
        {
            combo->addItem (name, ++index);
        }
    }
    else if (menuCreationLambda)
    {
        menuCreationLambda (*combo);
    }

    addAndMakeVisible (combo.get());

    combo->onChange = [&]
    {
        if (auto* c = dynamic_cast<juce::ComboBox*>(editor.get()))
            node.setProperty (property, c->getText(), &builder.getUndoManager());

        refresh();
    };

    editor = std::move (combo);

    proxy.addListener (this);
}

void StyleChoicePropertyComponent::refresh()
{
    const auto value = lookupValue();

    if (auto* combo = dynamic_cast<juce::ComboBox*>(editor.get()))
    {
        if (node == inheritedFrom)
        {
            proxy.referTo (node.getPropertyAsValue (property, &builder.getUndoManager()));
        }
        else
        {
            proxy.referTo (proxy);
            combo->setText (value.toString(), juce::dontSendNotification);
        }
    }

    repaint();
}

void StyleChoicePropertyComponent::valueChanged (juce::Value&)
{
    if (updating)
        return;

    juce::ScopedValueSetter<bool> updateFlag (updating, true);
    auto v = proxy.getValue().toString();

    if (auto* combo = dynamic_cast<juce::ComboBox*>(editor.get()))
    {
        if (combo->getText() != v)
            combo->setText (v, juce::sendNotificationSync);
    }

    if (property == IDs::lookAndFeel)
    {
        // this hack is needed, since the changing will have to trigger all colours a refresh
        // to fetch fallback colours
        if (auto* panel = findParentComponentOfClass<juce::PropertyPanel>())
            panel->refreshAll();
    }
    else
    {
        refresh();
    }
}


} // namespace foleys
