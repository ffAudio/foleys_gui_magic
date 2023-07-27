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

#include <juce_audio_processors/juce_audio_processors.h>

#include "../Visualisers/foleys_MagicPlotSource.h"
#include "../General/foleys_StringDefinitions.h"

namespace foleys
{

/**
 The MagicGuiState is the docking station for the MagicGUIBuilder.
 You can register properties or objects there, that your GuiItems will connect to.
 It is also the place, where the data for the visualisers is sent to, which are
 MagicPlotSources and MagicLevelSources.
 */
class MagicGUIState
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
    MagicGUIState();

    virtual ~MagicGUIState();

    /**
     Returns the root node for exposed properties for the GUI
     */
    juce::ValueTree getPropertyRoot();
    juce::ValueTree getPropertyRoot() const;

    /**
     Set the GUI DOM to create the GUI components from
     */
    void setGuiValueTree (const juce::ValueTree& dom);
    void setGuiValueTree (const char* data, int dataSize);
    void setGuiValueTree (const juce::File& file);

    /**
     Grants access to the gui tree. This is returned as reference so you are able to connect listeners to it.
     */
    juce::ValueTree& getGuiTree();

    juce::ValueTree& getValueTree();

    /**
     Set a file to save common settings for all instances
     */
    void setApplicationSettingsFile (juce::File file);

    /**
     This is a settings ValueTree that is stored globally for all plugin instances
     */
    juce::ValueTree& getSettings();

    /**
     Returns the IDs of AudioProcessorParameters for selection
     */
    virtual juce::StringArray getParameterNames() const;

    virtual juce::RangedAudioParameter* getParameter (const juce::String& paramID)
    { juce::ignoreUnused(paramID); return nullptr; }
    virtual std::unique_ptr<juce::SliderParameterAttachment>   createAttachment (const juce::String& paramID, juce::Slider&)
    { juce::ignoreUnused(paramID); return nullptr; }
    virtual std::unique_ptr<juce::ComboBoxParameterAttachment> createAttachment (const juce::String& paramID, juce::ComboBox&)
    { juce::ignoreUnused(paramID); return nullptr; }
    virtual std::unique_ptr<juce::ButtonParameterAttachment>   createAttachment (const juce::String& paramID, juce::Button&)
    { juce::ignoreUnused(paramID); return nullptr; }

    /**
     Return a hierarchical menu of the AudioParameters

     Implemented in MagicPluginState
     */
    virtual juce::PopupMenu createParameterMenu() const { return {}; }

    /**
     You can store a lambda that can be called from e.g. a TextButton.
     */
    void addTrigger (const juce::Identifier& triggerID, std::function<void()> function);

    /**
     Returns a lambda to be connected to your components
     */
    std::function<void()> getTrigger (const juce::Identifier& triggerID);

    /**
     Returns a property as value inside the ValueTreeState. The nodes are a colon separated list, the last component is the property name
     */
    juce::Value getPropertyAsValue (const juce::String& pathToProperty);

    /**
     Populates a menu with properties found in the persistent ValueTree
     */
    void populatePropertiesMenu (juce::ComboBox& comboBox) const;

    juce::PopupMenu createPropertiesMenu (juce::ComboBox& combo) const;

    juce::PopupMenu createTriggerMenu() const;

    juce::PopupMenu createAssetFilesMenu() const;

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

        if (auto* plot = dynamic_cast<MagicPlotSource*>(pointerToReturn))
            addBackgroundProcessing (plot);

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
     Registers background processing
     */
    void addBackgroundProcessing (MagicPlotSource* source);

    juce::MidiKeyboardState& getKeyboardState();

    /**
     Return the referenced AudioProcessor, if this state can provide one
     */
    virtual juce::AudioProcessor* getProcessor() { return nullptr; }

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    void setResourcesFolder (const juce::String& name);
    juce::File getResourcesFolder() const;

private:
    juce::File resourcesFolder;
#endif

private:

    void addParametersToMenu (const juce::AudioProcessorParameterGroup& group, juce::PopupMenu& menu, int& index) const;
    void addPropertiesToMenu (const juce::ValueTree& tree, juce::ComboBox& combo, juce::PopupMenu& menu, const juce::String& path) const;

    /**
     The ApplicationSettings is used for settings e.g. over many plugin instances.
     */
    SharedApplicationSettings settings;

    juce::ValueTree guiValueTree { IDs::magic };
    juce::ValueTree state        { "state" };

    juce::MidiKeyboardState keyboardState;

    std::map<juce::Identifier, std::function<void()>>       triggers;

    std::map<juce::Identifier, std::unique_ptr<ObjectBase>> advertisedObjects;

    juce::TimeSliceThread visualiserThread { "Visualiser Thread" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicGUIState)
};

} // namespace foleys
