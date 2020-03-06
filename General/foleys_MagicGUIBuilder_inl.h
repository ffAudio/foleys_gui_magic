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

template<class AppType>
MagicGUIBuilder<AppType>::MagicGUIBuilder (AppType& appToUse, MagicProcessorState* magicStateToUse)
: app (appToUse),
magicState (magicStateToUse)
{
    config = juce::ValueTree (IDs::magic);

    updateStylesheet();
}

template<class AppType>
void MagicGUIBuilder<AppType>::createDefaultGUITree (bool keepExisting)
{
    if (keepExisting && config.getChildWithName (IDs::view).isValid())
        return;

    auto rootNode = config.getOrCreateChildWithName (IDs::view, &undo);
    rootNode.setProperty (IDs::id, IDs::root, &undo);

    auto current = rootNode;

    if (magicState != nullptr)
    {
        auto plotNames = magicState->getPlotSourcesNames();

        if (plotNames.isEmpty() == false)
        {
            juce::StringArray colours { "orange", "blue", "red", "silver", "green", "cyan", "brown", "white" };
            int nextColour = 0;

            auto child = juce::ValueTree (IDs::view, {
                { IDs::id, "plot-view" },
                { IDs::styleClass, "plot-view" }});

            for (auto plotName : plotNames)
            {
                child.appendChild ({IDs::plot, {
                    { IDs::source, plotName },
                    { "plot-color", colours [nextColour++] }}}, nullptr);

                if (nextColour >= colours.size())
                    nextColour = 0;
            }

            current.appendChild (child, nullptr);

            juce::ValueTree parameters { IDs::view, {
                { IDs::styleClass, "parameters nomargin" }}};

            current.appendChild (parameters, nullptr);
            current = parameters;
        }

        createDefaultFromParameters (current, magicState->getProcessor().getParameterTree());
    }
}

template<class AppType>
void MagicGUIBuilder<AppType>::registerFactory (juce::Identifier type, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&, AppType&)> factory)
{
    if (factories.find (type) != factories.cend())
    {
        // You tried to add two factories with the same type name!
        // That cannot work, the second factory will be ignored.
        jassertfalse;
        return;
    }

    factories [type] = factory;
}

template<class AppType>
juce::StringArray MagicGUIBuilder<AppType>::getFactoryNames() const
{
    juce::StringArray names { IDs::view.toString() };

    names.ensureStorageAllocated (int (factories.size()));
    for (const auto& f : factories)
        names.add (f.first.toString());

    return names;
}

template<class AppType>
juce::StringArray MagicGUIBuilder<AppType>::getParameterNames() const
{
    if (magicState == nullptr)
        return {};

    return magicState->getParameterNames();
}

template<class AppType>
juce::StringArray MagicGUIBuilder<AppType>::getLevelSourcesNames() const
{
    if (magicState == nullptr)
        return {};

    return magicState->getLevelSourcesNames();
}

template<class AppType>
juce::StringArray MagicGUIBuilder<AppType>::getPlotSourcesNames() const
{
    if (magicState == nullptr)
        return {};

    return magicState->getPlotSourcesNames();
}

template<class AppType>
std::unique_ptr<Decorator> MagicGUIBuilder<AppType>::restoreNode (juce::Component& component, const juce::ValueTree& node)
{
    if (node.getType() == IDs::view)
    {
        auto item = std::make_unique<Container>(*this, node);
        for (auto childNode : node)
            item->addChildItem (restoreNode (*item, childNode));

        component.addAndMakeVisible (item.get());
        return item;
    }

    auto factory = factories [node.getType()];

    if (factory == nullptr)
    {
        DBG ("No factory for: " << node.getType().toString());
    }

    auto item = std::make_unique<Decorator> (*this, node, factory ? factory (node, app) : nullptr);
    component.addAndMakeVisible (item.get());

    return item;
}


} // namespace foleys
