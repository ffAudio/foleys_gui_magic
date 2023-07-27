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

#include "foleys_MagicPluginEditor.h"
#include "foleys_StringDefinitions.h"
#include "../State/foleys_MagicProcessorState.h"

namespace foleys
{

MagicPluginEditor::MagicPluginEditor (MagicProcessorState& stateToUse, std::unique_ptr<MagicGUIBuilder> builderToUse)
  : juce::AudioProcessorEditor (*stateToUse.getProcessor()),
    processorState (stateToUse),
    builder (std::move (builderToUse))
{
#if JUCE_MODULE_AVAILABLE_juce_opengl && FOLEYS_ENABLE_OPEN_GL_CONTEXT
    oglContext.attachTo (*this);
#endif

    if (builder.get() == nullptr)
    {
        builder = std::make_unique<MagicGUIBuilder>(processorState);
        builder->registerJUCEFactories();
        builder->registerJUCELookAndFeels();
    }

    auto guiTree = processorState.getGuiTree();
    if (guiTree.isValid())
        setConfigTree (guiTree);

    updateSize();

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE
    if (!guiTree.isValid() && processorState.getValueTree().isValid())
        processorState.getValueTree().addChild (builder->getConfigTree(), -1, nullptr);

    builder->attachToolboxToWindow (*this);
#endif
}

MagicPluginEditor::~MagicPluginEditor()
{
#if JUCE_MODULE_AVAILABLE_juce_opengl && FOLEYS_ENABLE_OPEN_GL_CONTEXT
    oglContext.detach();
#endif
}

void MagicPluginEditor::updateSize()
{
    const auto rootNode = builder->getGuiRootNode();

    int width = rootNode.getProperty (IDs::width, 600);
    int height = rootNode.getProperty (IDs::height, 400);

    bool resizable = builder->getStyleProperty (IDs::resizable, builder->getGuiRootNode());
    bool resizeCorner = builder->getStyleProperty (IDs::resizeCorner, builder->getGuiRootNode());

#if JUCE_IOS
    resizable = false;
    resizeCorner = false;
#endif

    if (resizable)
    {
        processorState.getLastEditorSize (width, height);

        auto maximalBounds = juce::Desktop::getInstance().getDisplays().getTotalBounds (true);
        int minWidth = rootNode.getProperty (IDs::minWidth, 10);
        int minHeight = rootNode.getProperty (IDs::minHeight, 10);
        int maxWidth = rootNode.getProperty (IDs::maxWidth, maximalBounds.getWidth());
        int maxHeight = rootNode.getProperty (IDs::maxHeight, maximalBounds.getHeight());
        double aspect = rootNode.getProperty (IDs::aspect, 0.0);
        setResizable (resizable, resizeCorner);
        setResizeLimits (minWidth, minHeight, maxWidth, maxHeight);
        if (aspect > 0.0)
            getConstrainer()->setFixedAspectRatio (aspect);
    }

    setSize (width, height);
}

void MagicPluginEditor::setConfigTree (const juce::ValueTree& config)
{
    jassert (config.isValid() && config.hasType(IDs::magic));

    // Set default values
    auto rootNode = config.getChildWithName (IDs::view);

    if (rootNode.isValid())
    {
        auto& undo = builder->getUndoManager();
        if (!rootNode.hasProperty(IDs::resizable)) rootNode.setProperty (IDs::resizable, true, &undo);
        if (!rootNode.hasProperty(IDs::resizeCorner)) rootNode.setProperty (IDs::resizeCorner, true, &undo);
    }

    builder->createGUI (*this);
    updateSize();
}

MagicGUIBuilder& MagicPluginEditor::getGUIBuilder()
{
    // This shouldn't be possible, since the builder instance is created if none was supplied...
    jassert (builder);

    return *builder;
}

void MagicPluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void MagicPluginEditor::resized()
{
    builder->updateLayout();

    processorState.setLastEditorSize (getWidth(), getHeight());
}

} // namespace foleys
