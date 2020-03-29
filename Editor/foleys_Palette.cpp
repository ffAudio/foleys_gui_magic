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


Palette::Palette (MagicGUIBuilder& builderToUse)
  : builder (builderToUse)
{
    addAndMakeVisible (paletteList);

    update();
}

void Palette::paint (juce::Graphics& g)
{
    g.fillAll (EditorColours::background);
}

void Palette::resized()
{
    paletteList.setBounds (getLocalBounds());
}

void Palette::update()
{
    paletteModel.setFactoryNames (builder.getFactoryNames());
    paletteList.updateContent();
}

//==============================================================================

void Palette::PaletteListModel::setFactoryNames (juce::StringArray names)
{
    factoryNames = names;
}

int Palette::PaletteListModel::getNumRows()
{
    return factoryNames.size();
}

void Palette::PaletteListModel::paintListBoxItem (int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected)
{
    auto b = juce::Rectangle<int> (1, 1, width - 2, height - 2).toFloat();

    const auto r = b.getHeight() / 2.0f;
    g.fillAll (EditorColours::background);

    if (rowIsSelected)
    {
        g.setColour (EditorColours::selectedBackground.withAlpha (0.5f));
        g.fillRoundedRectangle (b, r);
    }

    g.setColour (EditorColours::outline);
    g.drawRoundedRectangle (b, r, 1);

    const auto box = juce::Rectangle<int> (juce::roundToInt (r), 0, juce::roundToInt (width - 2 * r), height);
    g.setColour (EditorColours::text);
    g.drawFittedText (factoryNames [rowNumber], box, juce::Justification::left, 1);
    g.setColour (EditorColours::disabledText);
    g.drawFittedText (TRANS ("drag me"), box, juce::Justification::right, 1);
}

juce::var Palette::PaletteListModel::getDragSourceDescription (const juce::SparseSet<int> &rowsToDescribe)
{
    return juce::ValueTree {factoryNames [rowsToDescribe [0]]}.toXmlString();
}


} // namespace foleys
