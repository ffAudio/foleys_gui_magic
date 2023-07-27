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

namespace foleys
{


/**
 The ParameterAttachment is a class you can use to have a value synchronised with
 an AudioProcessorParameter. It takes care of updating either way, keeps the value
 thread safe in an atomic and allows setting lambdas to be executed when the value
 changes.
 */
template<typename ValueType>
class ParameterAttachment : private juce::AudioProcessorParameter::Listener,
                            private juce::AsyncUpdater
{
public:
    ParameterAttachment()
    {
    }

    virtual ~ParameterAttachment() override
    {
        detachFromParameter();
    }

    /**
     Thread safe way to read the current value.
     */
    ValueType getValue() const
    {
        return value.load();
    }

    /**
     Thread safe way to read the normalised value of the current value.
     */
    ValueType getNormalisedValue() const
    {
        if (parameter)
            return parameter->getNormalisableRange().convertTo0to1 (value.load());

        return value.load();
    }

    /**
     Set the value from a not normalised range.min..range.max value.
     */
    void setValue (ValueType newValue)
    {
        if (parameter)
            parameter->setValueNotifyingHost (parameter->getNormalisableRange().convertTo0to1 (newValue));
        else
            parameterValueChanged (0, juce::jlimit (0.0f, 1.0f, newValue));
    }

    /**
     Set the value from a normalised 0..1 value.
     */
    void setNormalisedValue (ValueType newValue)
    {
        if (parameter)
            parameter->setValueNotifyingHost (newValue);
        else
            parameterValueChanged (0, juce::jlimit (0.0f, 1.0f, newValue));
    }

    /**
     Make this value attached to the parameter with the supplied parameterID.
     */
    void attachToParameter (juce::RangedAudioParameter* parameterToUse)
    {
        detachFromParameter();

        if (parameterToUse)
        {
            parameter = parameterToUse;

            initialUpdate();

            parameter->addListener (this);
        }
    }

    void detachFromParameter()
    {
        if (parameter)
            parameter->removeListener (this);
    }

    /**
     Make sure to call this before you send changes (e.g. from mouseDown of your UI widget),
     otherwise the hosts automation will battle with your value changes.
     */
    void beginGesture()
    {
        if (parameter)
            parameter->beginChangeGesture();
    }

    /**
     Make sure to call this after finishing your changes (e.g. from mouseDown of your UI widget),
     this way the automation can take back control (like e.g. latch mode).
     */
    void endGesture()
    {
        if (parameter)
            parameter->endChangeGesture();
    }

    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        juce::ignoreUnused (parameterIndex);
        if (parameter)
            value.store (ValueType (parameter->convertFrom0to1 (newValue)));
        else
            value.store (ValueType (newValue));

        if (onParameterChanged)
            onParameterChanged();

        if (onParameterChangedAsync)
            triggerAsyncUpdate();
    }

    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override
    { juce::ignoreUnused (parameterIndex); juce::ignoreUnused (gestureIsStarting); }

    void handleAsyncUpdate() override
    {
        if (onParameterChangedAsync)
            onParameterChangedAsync();
    }

    /**
     Set this lambda to be called from whatever thread is updating the parameter
     */
    std::function<void()> onParameterChanged;

    /**
     Set this lambda to be called from the message thread via AsyncUpdater
     */
    std::function<void()> onParameterChangedAsync;

private:

    void initialUpdate()
    {
        if (parameter)
            value.store (ValueType (parameter->convertFrom0to1 (parameter->getValue())));
        else
            value.store (ValueType());

        if (onParameterChanged)
            onParameterChanged();
    }

    juce::RangedAudioParameter*         parameter = nullptr;
    std::atomic<ValueType>              value { ValueType() };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterAttachment)
};


} // namespace foleys
