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

class MagicGUIBuilder;

/**
 The Palette is the list of available components to be dragged into the tree or the GUI window.
 To be able to drop into the GUI directly, the edit mode must be enabled.
 */
class Palette  : public juce::Component
{
public:
    Palette (MagicGUIBuilder& builder);

    void paint (juce::Graphics& g) override;

    void resized() override;

    void update();

private:

    MagicGUIBuilder& builder;

    class PaletteListModel : public juce::ListBoxModel
    {
    public:
        PaletteListModel() = default;

        void setFactoryNames (juce::StringArray names);

        int getNumRows() override;
        void paintListBoxItem (int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected) override;
        juce::var getDragSourceDescription (const juce::SparseSet<int> &rowsToDescribe) override;

    private:
        juce::StringArray factoryNames;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PaletteListModel)
    };

    PaletteListModel        paletteModel;
    juce::ListBox           paletteList { {}, &paletteModel };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Palette)
};

} // namespace foleys
