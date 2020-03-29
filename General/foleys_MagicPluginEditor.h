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
 This is a generic AudioProcessorEditor, that is completely
 defined and styled by drag and drop. There is an XML representation,
 that can be baked into the project using the BinaryData of Projucer.
 */
class MagicPluginEditor  : public juce::AudioProcessorEditor,
                           public juce::DragAndDropContainer
{
public:
    MagicPluginEditor (MagicProcessorState& processorState, std::unique_ptr<MagicGUIBuilder> builder = {});

    MagicPluginEditor (MagicProcessorState& processorState, const char* data, const int dataSize, std::unique_ptr<MagicGUIBuilder> builder = {});

    ~MagicPluginEditor() override;

    /**
     Setup a GUI from a previously stored ValueTree

     @param gui the ValueTree that defines the GUI of the editor
     */
    void setConfigTree (const juce::ValueTree& gui);

    /**
     Setup a GUI from a previously stored ValueTree. This is the
     usual way to deploy your finished GUI via the Projucer:

     @code{.cpp}
     AudioProcessorEditor* MyAudioProcessor::createEditor()
     {
         auto* editor = new foleys::MagicPluginEditor (magicState);
         editor->restoreGUI (BinaryData::magic_xml, BinaryData::magic_xmlSize);
         return editor;
     }
     @endcode

     @param data points to the binary data of the XML file
     @param dataSize the number of bytes
     */
    void setConfigTree (const char* data, const int dataSize);

    void paint (juce::Graphics& g) override;

    void resized() override;

private:
    void initialise (const char* data = nullptr, const int dataSize = 0);

    std::unique_ptr<MagicGUIBuilder> createBuilderInstance();

    void updateSize();

#if JUCE_MODULE_AVAILABLE_juce_opengl && FOLEYS_ENABLE_OPEN_GL_CONTEXT
    juce::OpenGLContext oglContext;
#endif

    MagicProcessorState& processorState;

    std::unique_ptr<MagicGUIBuilder> builder;

    juce::TooltipWindow tooltip      { this };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicPluginEditor)
};

} // namespace foleys
