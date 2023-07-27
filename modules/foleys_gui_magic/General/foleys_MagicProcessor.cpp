/*
 ==============================================================================
    Copyright (c) 2021 - 2023 Foleys Finest Audio - Daniel Walz
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

#include "foleys_MagicProcessor.h"
#include "../Helpers/foleys_DefaultGuiTrees.h"

namespace foleys
{


MagicProcessor::MagicProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : juce::AudioProcessor (juce::AudioProcessor::BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{}

MagicProcessor::MagicProcessor (const BusesProperties& ioLayouts) : juce::AudioProcessor (ioLayouts)
{}

MagicProcessor::MagicProcessor (const std::initializer_list<const short[2]>& channelLayoutList) : juce::AudioProcessor (channelLayoutList)
{}

//==============================================================================
void MagicProcessor::initialiseBuilder (MagicGUIBuilder& builder)
{
    builder.registerJUCEFactories();
    builder.registerJUCELookAndFeels();
}

juce::ValueTree MagicProcessor::createGuiValueTree()
{
    juce::ValueTree magic {IDs::magic, {},
                           {juce::ValueTree { IDs::styles, {},
                                              {DefaultGuiTrees::createDefaultStylesheet()}}}};

    juce::ValueTree rootNode {IDs::view, {{ IDs::id, IDs::root }}};

    auto plotView = DefaultGuiTrees::createPlotView (magicState);
    if (plotView.isValid())
        rootNode.appendChild (plotView, nullptr);

    auto params = DefaultGuiTrees::createProcessorGui (getParameterTree());
    rootNode.appendChild (params, nullptr);

    magic.appendChild (rootNode, nullptr);

    return magic;
}

juce::AudioProcessorEditor* MagicProcessor::createEditor()
{
    magicState.updateParameterMap();

    auto builder = std::make_unique<MagicGUIBuilder>(magicState);
    initialiseBuilder (*builder);

    if (magicState.getGuiTree().getChildWithName (IDs::view).isValid() == false)
        magicState.setGuiValueTree (createGuiValueTree());

    return new MagicPluginEditor (magicState, std::move (builder));
}

//==============================================================================
void MagicProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    magicState.getStateInformation (destData);
}

void MagicProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    magicState.setStateInformation (data, sizeInBytes, getActiveEditor());

    postSetStateInformation();
}

} // namespace foleys
