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

Decorator::Decorator()
{
    setOpaque (margin == 0 && radius == 0);
}

Decorator::Decorator (std::unique_ptr<juce::Component> wrapped)
  : component (std::move (wrapped))
{
    setOpaque (margin == 0 && radius == 0);

    if (component.get() != nullptr)
        addAndMakeVisible (component.get());
}

void Decorator::setEditable (bool shouldEdit)
{
    setInterceptsMouseClicks (shouldEdit, true);

    if (component.get() != nullptr)
        component->setInterceptsMouseClicks (!shouldEdit, true);
}

void Decorator::configureDecorator (Stylesheet& stylesheet, const juce::ValueTree& node)
{
    auto bg = stylesheet.getProperty (IDs::backgroundColour, node);
    if (! bg.isVoid())
        backgroundColour = stylesheet.parseColour (bg.toString());

    auto bcVar = stylesheet.getProperty (IDs::borderColour, node);
    if (! bcVar.isVoid())
        borderColour = stylesheet.parseColour (bcVar.toString());

    auto borderVar = stylesheet.getProperty (IDs::border, node);
    if (! borderVar.isVoid())
        border = static_cast<float> (borderVar);

    auto marginVar = stylesheet.getProperty (IDs::margin, node);
    if (! marginVar.isVoid())
        margin = static_cast<float> (marginVar);

    auto paddingVar = stylesheet.getProperty (IDs::padding, node);
    if (! paddingVar.isVoid())
        padding = static_cast<float> (paddingVar);

    backgroundImage = stylesheet.getBackgroundImage (node);
    backgroundFill  = stylesheet.getBackgroundGradient (node);

    if (component)
        if (auto* lookAndFeel = stylesheet.getLookAndFeel (node))
            component->setLookAndFeel (lookAndFeel);
}

void Decorator::connectToState (const juce::String& paramID, juce::AudioProcessorValueTreeState& state)
{
    if (auto* slider = dynamic_cast<juce::Slider*>(component.get()))
    {
        sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, paramID, *slider);
    }
    else if (auto* combo = dynamic_cast<juce::ComboBox*>(component.get()))
    {
        if (auto* parameter = state.getParameter (paramID))
            combo->addItemList (parameter->getAllValueStrings(), 1);

        comboboxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, paramID, *combo);
    }
    else if (auto* button = dynamic_cast<juce::Button*>(component.get()))
    {
        button->setClickingTogglesState (true);
        buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, paramID, *button);
    }
}

void Decorator::paint (juce::Graphics& g)
{
    juce::Graphics::ScopedSaveState stateSave (g);

    const auto bounds = getLocalBounds().reduced (margin).toFloat();

    if (radius > 0.0f)
    {
        g.setColour (backgroundColour);

        if (backgroundFill.size() > 1)
            g.setGradientFill (juce::ColourGradient::vertical (backgroundFill.getFirst(), backgroundFill.getLast(), bounds));

        g.fillRoundedRectangle (bounds, radius);

        if (! backgroundImage.isNull())
            g.drawImage (backgroundImage, bounds);

        if (border > 0.0f)
        {
            g.setColour (borderColour);
            g.drawRoundedRectangle (bounds, radius, border);
        }
    }
    else
    {
        g.setColour (backgroundColour);

        if (backgroundFill.size() > 1)
            g.setGradientFill (juce::ColourGradient::vertical (backgroundFill.getFirst(), backgroundFill.getLast(), bounds));

        g.fillRect (bounds);

        if (! backgroundImage.isNull())
            g.drawImage (backgroundImage, bounds);

        if (border > 0.0f)
        {
            g.setColour (borderColour);
            g.drawRect (bounds, border);
        }
    }
}

juce::Rectangle<int> Decorator::getClientBounds() const
{
    return getLocalBounds().reduced (margin + padding);
}

void Decorator::resized()
{
    if (component.get() != nullptr)
        component->setBounds (getClientBounds());
}

juce::Component* Decorator::getWrappedComponent()
{
    return component.get();
}

}
