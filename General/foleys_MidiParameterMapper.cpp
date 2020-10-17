/*
 ==============================================================================
    Copyright (c) 2019-2020 Foleys Finest Audio Ltd. - Daniel Walz
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

void MidiParameterMapper::processMidiBuffer (juce::MidiBuffer& buffer)
{
    for (auto m : buffer)
    {
        if (m.getMessage().isController())
        {
            auto number = m.getMessage().getControllerNumber();
            auto value  = m.getMessage().getControllerValue() / 127.0f;
            auto bin = midiMapper.find (number);
            if (bin == midiMapper.end())
                continue;

            for (auto p : bin->second)
            {
                p->beginChangeGesture();
                p->setValueNotifyingHost (value);
                p->endChangeGesture();
            }
            lastController.store (number);
        }
    }
}

void MidiParameterMapper::mapMidiController (int cc, juce::RangedAudioParameter* parameter)
{
    if (parameter == nullptr)
        return;

    auto bin = midiMapper.find (cc);
    if (bin == midiMapper.end())
    {
        midiMapper [cc] = { parameter };
    }
    else
    {
        if (std::find (bin->second.begin(), bin->second.end(), parameter) != bin->second.end())
            return;

        bin->second.push_back (parameter);
    }
}

void MidiParameterMapper::unmapMidiController (int cc, const juce::String& parameterID)
{
    auto bin = midiMapper.find (cc);
    if (bin == midiMapper.end())
        return;

    auto& vector = bin->second;
    vector.erase (std::remove_if (vector.begin(), vector.end(), [parameterID](auto& p) { return p->paramID == parameterID; }),
                  vector.end());
}

void MidiParameterMapper::unmapAllMidiController (int cc)
{
    midiMapper.erase (cc);
}

int MidiParameterMapper::getLastController() const
{
    return lastController.load();
}


} // namespace foleys
