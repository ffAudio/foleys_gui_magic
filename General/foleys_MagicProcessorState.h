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
    struct ObjectBase {
        virtual ~ObjectBase() noexcept = default;
    };

    template <typename ToErase> class ErasedObject : public ObjectBase, public ToErase {
    public:
        template <typename... Ts>
        explicit ErasedObject(Ts &&... ts) : ToErase{std::forward<Ts>(ts)...} {}
    };

    template <typename T, typename... Ts>
    std::unique_ptr<ErasedObject<T>> makeErased(Ts &&... t) {
      return std::make_unique<ErasedObject<T>>(std::forward<Ts>(t)...);
    }

public:
    /**
     Create a MagicProcessorState to let the generated GUI communicate with the
     processor and it's internals.
     */
    MagicProcessorState (juce::AudioProcessor& processorToUse,
                         juce::AudioProcessorValueTreeState& stateToUse);

    ~MagicProcessorState();

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
     Populates a menu with options from SettableProperty
     */
    void populateSettableOptionsMenu (juce::ComboBox& comboBox, SettableProperty::PropertyType type) const;

    /**
     Create and add an object. The type to create needs to be added as template parameter, the arguments will be forwarded to the constructor.
     */
    template <typename T, typename... Ts>
    T* createAndAddObject (const juce::Identifier& objectID, Ts &&... t)
    {
        const auto& present = advertisedObjects.find (objectID);
        if (present != advertisedObjects.cend())
        {
            // You tried to add two MagicPlotSources with the same sourceID
            jassertfalse;
            return nullptr;
        }

        auto o = std::make_unique<ErasedObject<T>>(std::forward<Ts>(t)...);
        auto* pointerToReturn = o.get();
        advertisedObjects [objectID] = std::move (o);

        return pointerToReturn;
    }

    /**
     Returns all identifiers of objects, that can be casted to the given type.
     */
    template <typename ObjectType>
    juce::StringArray getObjectIDsByType() const
    {
        juce::StringArray identifiers;
        for (const auto& object : advertisedObjects)
            if (dynamic_cast<ObjectType*>(object.second.get()))
                identifiers.add (object.first.toString());

        return identifiers;
    }

    /**
     Return an object by objectID. The returned type needs to be specified as template parameter.
     If there is no object with that objectID, or the object is not of the selected type, this will return a nullptr.

     @param objectID is the ID to identify the object.
     */
    template <typename ObjectType>
    ObjectType* getObjectWithType (juce::Identifier objectID)
    {
        const auto& object = advertisedObjects.find (objectID);
        if (object != advertisedObjects.cend())
            return dynamic_cast<ObjectType*>(object->second.get());

        return nullptr;
    }

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
     Registers background processing
     */
    void addBackgroundProcessing (MagicPlotSource* source);

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

    std::map<juce::Identifier, std::function<void()>>             triggers;

    std::map<juce::Identifier, std::unique_ptr<ObjectBase>>       advertisedObjects;

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
