/*
 ==============================================================================
    Copyright (c) 2019-2021 Foleys Finest Audio - Daniel Walz
    All rights reserved.

    License for non-commercial projects:

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

    License for commercial products:

    To sell commercial products containing this module, you are required to buy a
    License from https://foleysfinest.com/developer/pluginguimagic/

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

namespace foleys
{

namespace IDs
{
    static juce::String mappings  { "mappings" };
    static juce::String mapping   { "mapping" };
    static juce::String cc        { "cc" };
}

MidiParameterMapper::MidiParameterMapper (MagicProcessorState& s) : state (s)
{
    settings->settings.addListener (this);
}

MidiParameterMapper::~MidiParameterMapper()
{
    settings->settings.removeListener (this);
}

void MidiParameterMapper::processMidiBuffer (juce::MidiBuffer& buffer)
{
    auto isLocked = mappingLock.tryEnter();

    for (auto m : buffer)
    {
        if (m.getMessage().isController())
        {
            auto number = m.getMessage().getControllerNumber();
            auto value  = m.getMessage().getControllerValue() / 127.0f;
            lastController.store (number);

            if (isLocked)
            {
                auto bin = midiMapper.find (number);
                if (bin == midiMapper.end())
                    continue;

                for (auto p : bin->second)
                {
                    p->beginChangeGesture();
                    p->setValueNotifyingHost (value);
                    p->endChangeGesture();
                }
            }
        }
    }

    if (isLocked)
        mappingLock.exit();
}

void MidiParameterMapper::mapMidiController (int cc, const juce::String& parameterID)
{
    auto mappings = settings->settings.getOrCreateChildWithName (IDs::mappings, nullptr);

    juce::ValueTree node { IDs::mapping, {{IDs::cc, cc}, {IDs::parameter, parameterID}} };
    mappings.appendChild (node, nullptr);
}

void MidiParameterMapper::unmapMidiController (int cc, const juce::String& parameterID)
{
    auto mappings = settings->settings.getChildWithName (IDs::mappings);
    if (! mappings.isValid())
        return;

    int index = 0;
    while (index < mappings.getNumChildren())
    {
        const auto& child = mappings.getChild (index);
        if (int (child.getProperty (IDs::cc, -1)) == cc && child.getProperty (IDs::parameter, juce::String()).toString() == parameterID)
            mappings.removeChild (child, nullptr);
        else
            ++index;
    }
}

void MidiParameterMapper::unmapAllMidiController (int cc)
{
    auto mappings = settings->settings.getChildWithName (IDs::mappings);
    if (! mappings.isValid())
        return;

    int index = 0;
    while (index < mappings.getNumChildren())
    {
        const auto& child = mappings.getChild (index);
        if (int (child.getProperty (IDs::cc, -1)) == cc)
            mappings.removeChild (child, nullptr);
        else
            ++index;
    }
}

int MidiParameterMapper::getLastController() const
{
    return lastController.load();
}

void MidiParameterMapper::recreateMidiMapper()
{
    auto mappings = settings->settings.getChildWithName (IDs::mappings);
    if (! mappings.isValid())
        return;

    MidiMapping newMapping;

    for (auto item : mappings)
    {
        int  ccNum   = item.getProperty (IDs::cc, -1);
        auto paramID = item.getProperty (IDs::parameter, juce::String()).toString();
        if (ccNum < 1 || paramID.isEmpty())
            continue;

        auto* parameter = state.getParameter (paramID);
        if (parameter == nullptr)
            continue;

        newMapping [ccNum].push_back (parameter);
    }

    juce::ScopedLock lock (mappingLock);
    midiMapper = newMapping;
}

void MidiParameterMapper::valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&)
{
    recreateMidiMapper();
}

void MidiParameterMapper::valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int)
{
    recreateMidiMapper();
}

void MidiParameterMapper::valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&)
{
    recreateMidiMapper();
}

} // namespace foleys
