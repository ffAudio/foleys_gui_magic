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

MagicPluginEditor::MagicPluginEditor (MagicProcessorState& stateToUse)
  : juce::AudioProcessorEditor (stateToUse.getProcessor()),
    processorState (stateToUse)
{
    builder.registerJUCEFactories();
    builder.registerJUCELookAndFeels();

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    magicToolBox = std::make_unique<ToolBox>(getTopLevelComponent());
#endif

    setResizable (true, true);

    setSize (600, 400);
}

MagicPluginEditor::MagicPluginEditor (MagicProcessorState& stateToUse, const char* data, const int dataSize)
  : juce::AudioProcessorEditor (stateToUse.getProcessor()),
    processorState (stateToUse)
{
    builder.registerJUCEFactories();
    builder.registerJUCELookAndFeels();

    restoreGUI (data, dataSize);
    createDefaultGUI (true);

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    magicToolBox = std::make_unique<ToolBox>(getTopLevelComponent());
#endif

    setResizable (true, true);

    setSize (600, 400);
}

void MagicPluginEditor::restoreGUI (const juce::ValueTree& gui)
{
    builder.restoreGUI (gui);
}

void MagicPluginEditor::restoreGUI (const char* data, const int dataSize)
{
    juce::String text (data, dataSize);
    auto gui = juce::ValueTree::fromXml (text);
    restoreGUI (gui);
}

void MagicPluginEditor::createDefaultGUI (bool keepExisting)
{
    builder.createDefaultGUITree (keepExisting);
}

void MagicPluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void MagicPluginEditor::resized()
{
    builder.updateLayout();
}

} // namespace foleys
