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

#include "foleys_ParameterManager.h"
#include "foleys_MidiParameterMapper.h"

namespace foleys
{

/**
The MagicProcessorState is a subclass of MagicGUIState, that adds AudioProcessor specific functionality.
 It allows for instance connecting to AudioProcessorParameters and supplies a default XML tree* of components
built from the getParameterTree() from the AudioProcessor.
*/
class MagicProcessorState : public MagicGUIState,
                            private juce::Timer
{
public:
    /**
     Create a MagicProcessorState to let the generated GUI communicate with the
     processor and it's internals.
     */
    MagicProcessorState (juce::AudioProcessor& processorToUse);
    ~MagicProcessorState() override;

    /**
     Returns the IDs of AudioProcessorParameters for selection
     */
    juce::StringArray getParameterNames() const override;

    /**
     Return a hierarchical menu of the AudioParameters
     */
    juce::PopupMenu createParameterMenu() const override;

    /**
     Calling this in the processBlock() will store the values from AudioPlayHead into the state, so it can be used in the GUI.
     To enable this call setPlayheadUpdateFrequency (frequency) with an appropriate value
     */
    void updatePlayheadInformation (juce::AudioPlayHead* playhead);

    /**
     Starts the timer to fetch the playhead values from the audio thread
     */
    void setPlayheadUpdateFrequency (int frequency);

    /**
     Allows the editor to set its last size to resore next time
     */
    void setLastEditorSize (int  width, int  height);
    bool getLastEditorSize (int& width, int& height);

    /**
     This method will serialise the plugin state from AudioProcessorValueTreeState for
     the host to save in the session

     @param destData is the memory block to fill
     */
    void getStateInformation (juce::MemoryBlock& destData);

    /**
     This method restores the plugin state using the AudioProcessorValueTreeState.
     If you supply a pointer to the editor (using getActiveEditor()) the last size
     is automatically restored.

     @param data is a pointer to the original data
     @param sizeInBytes is the length of the data
     @param editor is an optional pointer to the editor to apply the last size to
     */
    void setStateInformation (const void* data, int sizeInBytes, juce::AudioProcessorEditor* editor = nullptr);

    /**
     Returns a parameter for a parameter ID
     */
    juce::RangedAudioParameter* getParameter (const juce::String& paramID) override;
    void updateParameterMap();

    std::unique_ptr<juce::SliderParameterAttachment>   createAttachment (const juce::String& paramID, juce::Slider& slider) override;
    std::unique_ptr<juce::ComboBoxParameterAttachment> createAttachment (const juce::String& paramID, juce::ComboBox& combobox) override;
    std::unique_ptr<juce::ButtonParameterAttachment>   createAttachment (const juce::String& paramID, juce::Button& button) override;

    juce::AudioProcessor* getProcessor() override;

    /**
     Send the midi data to the keyboard and to the MidiLearn mapper.

     @param buffer the midi buffer from processBlock
     @param numSamples the number of samples in the corresponding audio buffer
     @param injectIndirectEvents if true key presses from the GUI are added to the midi stream
     */
    void processMidiBuffer (juce::MidiBuffer& buffer, int numSamples, bool injectIndirectEvents=true);

    /**
     Connects a midi controller CC to a parameter for MIDI learn
     */
    void mapMidiController (int cc, const juce::String& parameterID);

    /**
     Returns the last moved controller for MIDI learn
     */
    int  getLastController() const;

private:

    void addParametersToMenu (const juce::AudioProcessorParameterGroup& group, juce::PopupMenu& menu, int& index) const;

    /**
     This creates a hierarchical DOM according to the parameters defined in an AudioProcessor
     */
    void createDefaultFromParameters (juce::ValueTree& node, const juce::AudioProcessorParameterGroup& tree) const;

    void timerCallback() override;

    juce::AudioProcessor& processor;

    ParameterManager    parameters { processor };
    MidiParameterMapper midiMapper { *this };

    std::atomic<double> bpm;
    std::atomic<int>    timeSigNumerator;
    std::atomic<int>    timeSigDenominator;
    std::atomic<double> timeInSeconds;
    std::atomic<bool>   isPlaying;
    std::atomic<bool>   isRecording;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicProcessorState)
};

} // namespace foleys
