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

class MagicProcessorState
{
public:
    MagicProcessorState (juce::AudioProcessor& processorToUse,
                         juce::AudioProcessorValueTreeState& stateToUse);

    /**
     Add a MagicLevelSource to measure the level at any point in your DSP. For instance you can have
     one MagicLevelSource at the beginning of your processing and one at the end to show the user
     the input and output level.
     */
    void addLevelSource (const juce::Identifier& sourceID, std::unique_ptr<MagicLevelSource> source);
    MagicLevelSource* getLevelSource (const juce::Identifier& sourceID);

    /**
     Add a MagicPlotSource to generate a plot for visualisation. There are some plots ready made
     like the MagicFilterPlot to show an IIR frequency response plot.
     */
    void addPlotSource (const juce::Identifier& sourceID, std::unique_ptr<MagicPlotSource> source);

    /**
     Use this to lookup a MagicPlotSource. Since they can only be added and never removed, it makes
     sense to have the result as member pointer so you can push the data there.
     */
    MagicPlotSource* getPlotSource (const juce::Identifier& sourceID);

    juce::AudioProcessor& getProcessor();
    juce::AudioProcessorValueTreeState& getValueTreeState();

private:

    juce::AudioProcessor& processor;
    juce::AudioProcessorValueTreeState& state;

    std::map<juce::Identifier, std::unique_ptr<MagicLevelSource>> levelSources;
    std::map<juce::Identifier, std::unique_ptr<MagicPlotSource>>  plotSources;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicProcessorState)
};

} // namespace foleys
