/*
 ==============================================================================
    Copyright (c) 2021-2023 Foleys Finest Audio - Daniel Walz
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

#include "foleys_DefaultGuiTrees.h"

namespace foleys
{


juce::ValueTree DefaultGuiTrees::createDefaultDocument (juce::ValueTree gui)
{
    return { IDs::magic, {},
        {
            juce::ValueTree (IDs::styles, {}, { DefaultGuiTrees::createDefaultStylesheet() }),
            gui
        }
    };
}

juce::ValueTree DefaultGuiTrees::createHelloWorld()
{
    return {IDs::view, {{"id", "root"}},
        {
            {"Label", {
                {"text", "Hello world!"},
                {"font-size", "25"},
                {"justification", "centred"}
            }}
        }};
}

void DefaultGuiTrees::createDefaultFromParameters (juce::ValueTree& node, const juce::AudioProcessorParameterGroup& tree)
{
    for (const auto& sub : tree.getSubgroups (false))
    {
        auto child = juce::ValueTree (IDs::view, {
            {IDs::caption, sub->getName()},
            {IDs::styleClass, "group"}});

        createDefaultFromParameters (child, *sub);
        node.appendChild (child, nullptr);
    }

    for (const auto& param : tree.getParameters (false))
    {
        auto child = juce::ValueTree (IDs::slider);
        if (dynamic_cast<juce::AudioParameterChoice*>(param) != nullptr)
            child = juce::ValueTree (IDs::comboBox);
        else if (dynamic_cast<juce::AudioParameterBool*>(param) != nullptr)
            child = juce::ValueTree (IDs::toggleButton);

        child.setProperty (IDs::caption, param->getName (64), nullptr);
        if (const auto* parameterWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(param))
            child.setProperty (IDs::parameter, parameterWithID->paramID, nullptr);

        node.appendChild (child, nullptr);
    }
}

juce::ValueTree DefaultGuiTrees::createProcessorGui (const juce::AudioProcessorParameterGroup& tree)
{
    juce::ValueTree params { IDs::view, {
        { IDs::styleClass, "parameters nomargin" }}};

    createDefaultFromParameters (params, tree);

    return params;
}

juce::ValueTree DefaultGuiTrees::createPlotView (const MagicGUIState& magicState)
{
    auto plotNames = magicState.getObjectIDsByType<MagicPlotSource>();

    if (plotNames.isEmpty())
        return {};

    juce::StringArray colours { "orange", "blue", "red", "silver", "green", "cyan", "brown", "white" };
    int nextColour = 0;

    juce::ValueTree child { IDs::view, {
        { IDs::id, "plot-view" },
        { IDs::styleClass, "plot-view" }}};

    for (auto plotName : plotNames)
    {
        child.appendChild ({IDs::plot, {
            { IDs::source, plotName },
            { IDs::styleClass, "transparent" },
            { "plot-color", colours [nextColour++] }}}, nullptr);

        if (nextColour >= colours.size())
            nextColour = 0;
    }

    return child;
}

juce::ValueTree DefaultGuiTrees::createDefaultStylesheet()
{
    juce::ValueTree style (IDs::style, {{ IDs::name, "default" }},
    {
        { IDs::nodes, {} },
        { IDs::classes, {}, {
            { "plot-view", {
                { IDs::border, 2 },
                { IDs::backgroundColour, "black" },
                { IDs::borderColour, "silver" },
                { IDs::display, IDs::contents }
            } },
            { "nomargin", {
                { IDs::margin, 0 },
                { IDs::padding, 0 },
                { IDs::border, 0 }} },
            { "group", {
                { IDs::margin, 5 },
                { IDs::padding, 5 },
                { IDs::border, 2 },
                { IDs::flexDirection, IDs::flexDirColumn }} },
            { "transparent", {
                { IDs::backgroundColour, "transparentblack" }} }
        } },
        { IDs::types, {}, {
            { "Slider", {{ IDs::border, 0 }, { "slider-textbox", "textbox-below" }} },
            { "ToggleButton", {{ IDs::border, 0 }, { IDs::maxHeight, 50 }, { IDs::captionSize, 0 }, { "text", "Active" }} },
            { "TextButton", {{ IDs::border, 0 }, { IDs::maxHeight, 50 }, { IDs::captionSize, 0 }} },
            { "ComboBox", {{ IDs::border, 0 }, { IDs::maxHeight, 50 }, { IDs::captionSize, 0 }} },
            { "Plot", {{ IDs::border, 0 }, { IDs::margin, 0 }, { IDs::padding, 0 }, { IDs::backgroundColour, "00000000" }, {IDs::radius, 0}} },
            { "XYDragComponent", {{ IDs::border, 0 }, { IDs::margin, 0 }, { IDs::padding, 0 }, { IDs::backgroundColour, "00000000" }, {IDs::radius, 0}} }
        } }
    });

    return style;
}

}
