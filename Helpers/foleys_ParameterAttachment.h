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


template<typename ValueType>
class ParameterAttachment : private juce::AudioProcessorValueTreeState::Listener,
                            private juce::AsyncUpdater
{
public:
    ParameterAttachment (juce::AudioProcessorValueTreeState& stateToUse)
      : state (stateToUse)
    {
    }
    
    virtual ~ParameterAttachment()
    {
        detachFromParameter();
    }

    ValueType getValue() const
    {
        return value.load();
    }

    float getNormalisedValue() const
    {
        if (parameter)
            return parameter->getNormalisableRange().convertTo0to1 (value.load());

        return value.load();
    }

    void setNormalisedValue (ValueType newValue)
    {
        if (parameter)
            parameter->setValueNotifyingHost (newValue);
        else
            parameterChanged (paramID, juce::jlimit (0.0f, 1.0f, newValue));
    }

    void attachToParameter (const juce::String& parameterID)
    {
        detachFromParameter();

        paramID = parameterID;

        parameter = dynamic_cast<juce::RangedAudioParameter*>(state.getParameter (paramID));
        // Oh uh, tried to attach to a non existing parameter
        jassert (parameter != nullptr);

        initialUpdate();

        state.addParameterListener (paramID, this);
    }

    void detachFromParameter()
    {
        if (paramID.isNotEmpty())
            state.removeParameterListener (paramID, this);
    }

    void beginGesture()
    {
        if (parameter)
            parameter->beginChangeGesture();
    }

    void endGesture()
    {
        if (parameter)
            parameter->endChangeGesture();
    }

    void parameterChanged (const juce::String& parameterID, float newValue) override
    {
        value.store (newValue);

        if (onParameterChanged)
            onParameterChanged();

        if (onParameterChangedAsync)
            triggerAsyncUpdate();
    }

    void handleAsyncUpdate() override
    {
        if (onParameterChangedAsync)
            onParameterChangedAsync();
    }

    /**
     Set this lambda to be called from whatever thread is updating the parameter
     */
    std::function<void()>               onParameterChanged;

    /**
     Set this lambda to be called from the message thread via AsyncUpdater
     */
    std::function<void()>               onParameterChangedAsync;

private:

    void initialUpdate()
    {
        value.store (ValueType (*state.getRawParameterValue (paramID)));
        if (onParameterChanged)
            onParameterChanged();
    }

    juce::AudioProcessorValueTreeState& state;
    juce::RangedAudioParameter*         parameter = nullptr;
    std::atomic<ValueType>              value;
    juce::String                        paramID;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterAttachment)
};


} // namespace foleys
