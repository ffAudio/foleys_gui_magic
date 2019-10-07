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


CreationPanels::CreationPanels (MagicBuilder& builderToUse)
  : builder (builderToUse)
{
    addAndMakeVisible (concertina);

    concertina.addPanel (0, &palette, false);
    setConcertinaHeaderFor (TRANS ("Components"), palette);

    concertina.addPanel (1, &parameters, false);
    setConcertinaHeaderFor (TRANS ("Parameters"), parameters);

    concertina.addPanel (2, &sources, false);
    setConcertinaHeaderFor (TRANS ("Visualisers"), sources);

    concertina.expandPanelFully (&palette, false);

    update();
}

void CreationPanels::paint (juce::Graphics& g)
{
    g.fillAll (EditorColours::background);
}

void CreationPanels::resized()
{
    concertina.setBounds (getLocalBounds());
}

void CreationPanels::update()
{
    paletteModel.setFactoryNames (builder.getFactoryNames());
    palette.updateContent();
}

void CreationPanels::setConcertinaHeaderFor (juce::String name, juce::Component& component)
{
    auto* header = new juce::TextButton (name);
    header->onClick = [&] { concertina.expandPanelFully (&component, true); };
    header->setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);
    concertina.setCustomPanelHeader (&component, header, true);
}

//==============================================================================

void CreationPanels::PaletteListModel::setFactoryNames (juce::StringArray names)
{
    factoryNames = names;
}

int CreationPanels::PaletteListModel::getNumRows()
{
    return factoryNames.size();
}

void CreationPanels::PaletteListModel::paintListBoxItem (int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected)
{
    g.fillAll (rowIsSelected ? EditorColours::selectedBackground.withAlpha (0.5f) : EditorColours::background);
    g.setColour (EditorColours::text);
    g.drawFittedText (factoryNames [rowNumber], 0, 0, width, height, juce::Justification::left, 1);
}

juce::var CreationPanels::PaletteListModel::getDragSourceDescription (const juce::SparseSet<int> &rowsToDescribe)
{
    return juce::ValueTree {factoryNames [rowsToDescribe [0]]}.toXmlString();
}


} // namespace foleys
