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

#include <juce_audio_processors/juce_audio_processors.h>

#pragma once

namespace foleys
{

/**
 This is a convenience class to create a plugin using PluginGuiMagic. It has all wired up for you,
 The MagicPluginEditor for you to design or use the generic default, the loading and saving of the state
 and many more.

 In the createGuiValueTree() you can create the GUI ValueTree either by using
 \code{.cpp}
 juce::ValueTree EqualizerExampleAudioProcessor::createGuiValueTree()
 {
     // generate a default GUI deduced from Parameters, ParameterGroups and MagicPlotSources
     return magicState.createDefaultGUITree();

     // or you load an existing one from BinaryData
     juce::String text (BinaryData::magic_xml, BinaryData::magic_xmlSize);
     return juce::ValueTree::fromXml (text);
 }
 \endcode
 */
class MagicProcessor  : public juce::AudioProcessor
{
public:
    /**
     Creates a MagicProcessor, that inherits juce::AudioProcessor and adds connections to the editable GUI.

     The floating editor toolbox allows auto save. To use that add this macro in the constructor to let the editor
     know the location of the source files:
     \code{.cpp}
     FOLEYS_SET_SOURCE_PATH(__FILE__);
     \endcode
     */
    MagicProcessor();
    MagicProcessor (const BusesProperties& ioLayouts);
    MagicProcessor (const std::initializer_list<const short[2]>& channelLayoutList);

    /**
     Override that method to initialise the builder, register your own bespoke components or LookAndFeel classes.
     If you override this and you want to use the bundled components don't forget to call those two lines, otherwise you
     won't have any Components or LookAndFeels to choose from
     \code{.cpp}
     builder.registerJUCEFactories();
     builder.registerJUCELookAndFeels();
     \endcode
     */
    virtual void initialiseBuilder (MagicGUIBuilder& builder);

    /**
     This method is called to create the GUI. The default implementation will come up with a ValueTree containing
     a default Stylesheet and populate the GUI components from the AudioProcessorParameters it finds using
     getParameterTree() as well as getting the MagicPlotSources.

     You can override this method with your bespoke algorithm to create a ValueTree or to load your ValueTree
     from BinaryData.
     */
    virtual juce::ValueTree createGuiValueTree();

    /**
     If there is anything you need to do after a new state was loaded you can override this method
     */
    virtual void postSetStateInformation() {}

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override                           { return 1; }
    int getCurrentProgram() override                        { return 0; }
    void setCurrentProgram (int index) override             { juce::ignoreUnused (index); }
    const juce::String getProgramName (int index) override  { juce::ignoreUnused (index); return "default"; }
    void changeProgramName (int index, const juce::String& newName) override { juce::ignoreUnused (index, newName); }

    //==============================================================================
#ifdef JucePlugin_Name
    const juce::String getName() const override { return JucePlugin_Name; }
#endif

#ifdef JucePlugin_WantsMidiInput
    bool acceptsMidi() const override
    {
       #if JucePlugin_WantsMidiInput
        return true;
       #else
        return false;
       #endif
    }
#endif

#ifdef JucePlugin_ProducesMidiOutput
    bool producesMidi() const override
    {
       #if JucePlugin_ProducesMidiOutput
        return true;
       #else
        return false;
       #endif
    }
#endif

#ifdef JucePlugin_IsMidiEffect
    bool isMidiEffect() const override
    {
       #if JucePlugin_IsMidiEffect
        return true;
       #else
        return false;
       #endif
    }
#endif

protected:
    MagicProcessorState magicState { *this };

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    /**
     In your MagicProcessor call this macro to set the toolbox save path to either "Sources" or if there is a sibling to "Resources"
     \code{.cpp}
     FOLEYS_SET_SOURCE_PATH (__FILE__);
     \endcode
     */
    #define FOLEYS_SET_SOURCE_PATH(source) magicState.setResourcesFolder (source);
#else
    #define FOLEYS_SET_SOURCE_PATH(source) juce::ignoreUnused (source);
#endif

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicProcessor)
};

} // namespace foleys
