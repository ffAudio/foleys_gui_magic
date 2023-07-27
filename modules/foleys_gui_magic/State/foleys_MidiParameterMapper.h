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

namespace foleys
{

/**
 The MidiParameterMapper allows to connect CC values to RangedAudioParameters
 */
class MidiParameterMapper  : private juce::ValueTree::Listener
{
public:
    MidiParameterMapper (MagicProcessorState& state);
    ~MidiParameterMapper() override;

    /*!
     * Get Midi CC messages and set parameters accordingly
     * @param buffer the last midi events
     */
    void processMidiBuffer (juce::MidiBuffer& buffer);

    /*!
     * Map a MIDI CC to a parameter
     * @param cc the MIDI CC number to map
     * @param parameterID the parameterID to map to
     */
    void mapMidiController (int cc, const juce::String& parameterID);

    /*!
     * Remove a specific mapping
     * @param cc the MIDI CC number to map
     * @param parameterID the parameterID to unmap
     */
    void unmapMidiController (int cc, const juce::String& parameterID);

    /*!
     * Remove all mappings from a specific CC controller
     * @param cc the MIDI CC number to unmap
     */
    void unmapAllMidiController (int cc);

    /*!
     * @return the last touched MIDI controller so it can be mapped
     */
    int  getLastController() const;

    /*!
     * Grant access to the ValueTree to save or restore the mappings manually
     * @return the ValueTree containing the mappings
     */
    juce::ValueTree getMappingSettings();

private:
    void recreateMidiMapper();

    void valueTreeChildAdded (juce::ValueTree& parentTree,
                              juce::ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved (juce::ValueTree& parentTree, juce::ValueTree&, int) override;
    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override;


    using MidiMapping=std::map<int, std::vector<juce::RangedAudioParameter*>>;

    SharedApplicationSettings   settings;
    juce::CriticalSection       mappingLock;

    MagicProcessorState&        state;
    std::atomic<int>            lastController { -1 };
    MidiMapping                 midiMapper;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiParameterMapper)
};

} // namespace foleys
