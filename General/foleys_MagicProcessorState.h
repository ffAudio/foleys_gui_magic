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

/**
 The MagicProcessorState is the docking station for the MagicPluginEditor.
 It grants access to the processor itself and the AudioProcessorValueTreeState.
 It is also the place, where the data for the visualisers is sent to, which are
 MagicPlotSources and MagicLevelSources.
 */
class MagicProcessorState   : private juce::Timer
{
public:
    /**
     Create a MagicProcessorState to let the generated GUI communicate with the
     processor and it's internals.
     */
    MagicProcessorState (juce::AudioProcessor& processorToUse,
                         juce::AudioProcessorValueTreeState& stateToUse);

    ~MagicProcessorState();

    /**
     Add a MagicLevelSource to measure the level at any point in your DSP. For instance you can have
     one MagicLevelSource at the beginning of your processing and one at the end to show the user
     the input and output level.
     */
    MagicLevelSource* addLevelSource (const juce::Identifier& sourceID, std::unique_ptr<MagicLevelSource> source);
    MagicLevelSource* getLevelSource (const juce::Identifier& sourceID);

    /**
     Add a MagicPlotSource to generate a plot for visualisation. There are some plots ready made
     like the MagicFilterPlot to show an IIR frequency response plot.
     */
    MagicPlotSource* addPlotSource (const juce::Identifier& sourceID, std::unique_ptr<MagicPlotSource> source);

    /**
     Use this to lookup a MagicPlotSource. Since they can only be added and never removed, it makes
     sense to have the result as member pointer so you can push the data there.
     */
    MagicPlotSource* getPlotSource (const juce::Identifier& sourceID);

    /**
     Add a function to be connected to e.g. Buttons
     */
    void addTrigger (const juce::Identifier& triggerID, std::function<void()> function);

    std::function<void()> getTrigger (const juce::Identifier& triggerID);

    /**
     Returns a property as value inside the ValueTreeState. The nodes are a colon separated list, the last component is the property name
     */
    juce::Value getPropertyAsValue (const juce::String& pathToProperty);

    /**
     Returns the root node for exposed properties for the GUI
     */
    juce::ValueTree getPropertyRoot() const;

    /**
     Returns the IDs of AudioProcessorParameters for selection
     */
    juce::StringArray getParameterNames() const;

    /**
     Returns the IDs of MagicLevelSources for selection
     */
    juce::StringArray getLevelSourcesNames() const;

    /**
     Returns the IDs of MagicPlotSources for selection
     */
    juce::StringArray getPlotSourcesNames() const;

    /**
     Populates a menu with options from SettableProperty
     */
    void populateSettableOptionsMenu (juce::ComboBox& comboBox, SettableProperty::PropertyType type) const;

    /**
     Call this method in your prepareToPlay implementation, to allow th visualisers to be
     properly setup
     */
    void prepareToPlay (double sampleRate, int samplesPerBlockExpected);

    /**
     Send the midi data to the keyboard. This is only needed, if you added a MidiKeyboardComponent.

     @param buffer the midi buffer from processBlock
     @param numSamples the number of samples in the corresponding audio buffer
     @param injectIndirectEvents if true key presses from the GUI are added to the midi stream
     */
    void processMidiBuffer (juce::MidiBuffer& buffer, int numSamples, bool injectIndirectEvents=true);


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

    juce::AudioProcessor& getProcessor();
    juce::AudioProcessorValueTreeState& getValueTreeState();
    juce::MidiKeyboardState& getKeyboardState();

private:

    void addParametersToMenu (const juce::AudioProcessorParameterGroup& group, juce::PopupMenu& menu, int& index) const;
    void addPropertiesToMenu (const juce::ValueTree& tree, juce::ComboBox& combo, juce::PopupMenu& menu, const juce::String& path) const;

    void timerCallback() override;

    juce::AudioProcessor& processor;
    juce::AudioProcessorValueTreeState& state;

    juce::MidiKeyboardState keyboardState;

    std::map<juce::Identifier, std::unique_ptr<MagicLevelSource>> levelSources;
    std::map<juce::Identifier, std::unique_ptr<MagicPlotSource>>  plotSources;
    std::map<juce::Identifier, std::function<void()>>             triggers;

    std::atomic<double> bpm;
    std::atomic<int>    timeSigNumerator;
    std::atomic<int>    timeSigDenominator;
    std::atomic<double> timeInSeconds;
    std::atomic<bool>   isPlaying;
    std::atomic<bool>   isRecording;

    juce::TimeSliceThread visualiserThread { "Visualiser Thread" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicProcessorState)
};

} // namespace foleys
