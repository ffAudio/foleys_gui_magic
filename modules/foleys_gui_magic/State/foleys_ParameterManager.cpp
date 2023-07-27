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

#pragma once

#include "foleys_ParameterManager.h"

namespace foleys
{

juce::Identifier ParameterManager::nodeName  { "PARAM" };
juce::Identifier ParameterManager::nodeId    { "id" };
juce::Identifier ParameterManager::nodeValue { "value" };


ParameterManager::ParameterManager (juce::AudioProcessor& p)
  : processor (p)
{
    updateParameterMap();
}

juce::RangedAudioParameter* ParameterManager::getParameter (const juce::String& paramID)
{
    auto p = parameterLookup.find (paramID);
    if (p != parameterLookup.end())
        return p->second;

    return nullptr;
}

void ParameterManager::updateParameterMap()
{
    for (auto* parameter : processor.getParameters())
        if (auto* withID = dynamic_cast<juce::RangedAudioParameter*>(parameter))
            parameterLookup [withID->paramID] = withID;
}

juce::StringArray ParameterManager::getParameterNames() const
{
    juce::StringArray names;
    for (auto& parameter : parameterLookup)
        names.add (parameter.second->paramID);

    return names;
}

void ParameterManager::saveParameterValues (juce::ValueTree& tree)
{
    updateParameterMap();

    for (auto& parameter : parameterLookup)
    {
        auto node = tree.getChildWithProperty (nodeId, parameter.first);
        if (node.isValid())
            node.setProperty (nodeValue, parameter.second->convertFrom0to1 (parameter.second->getValue()), nullptr);
        else
            tree.appendChild ({ nodeName, {
                { nodeId, parameter.second->paramID },
                { nodeValue, parameter.second->convertFrom0to1 (parameter.second->getValue()) }}}, nullptr);
    }
}

void ParameterManager::loadParameterValues (juce::ValueTree& tree)
{
    updateParameterMap();

    for (const auto& child : tree)
    {
        if (child.getType() == nodeName)
        {
            if (! (child.hasProperty (nodeId) && child.hasProperty (nodeValue)))
                continue;

            auto paramID = child.getProperty (nodeId).toString();
            if (auto* parameter = getParameter (paramID))
                parameter->setValueNotifyingHost (parameter->convertTo0to1 (child.getProperty (nodeValue)));
        }
    }
}



} // namespace foleys
