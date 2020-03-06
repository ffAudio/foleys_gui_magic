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

class MouseLambdas : public juce::MouseListener
{
public:
    MouseLambdas() = default;

    void attachTo (juce::Component* component)
    {
        if (listenedComponent)
            listenedComponent->removeMouseListener (this);

        listenedComponent = component;

        if (listenedComponent)
            listenedComponent->addMouseListener (this, false);
    }

    ~MouseLambdas()
    {
        listenedComponent->removeMouseListener (this);
    }

    void mouseDown (const juce::MouseEvent& event) override
    {
        if (onMouseDown)
            onMouseDown (event);
    }

    void mouseUp (const juce::MouseEvent& event) override
    {
        if (onMouseUp)
            onMouseUp (event);
    }

    void mouseDoubleClick (const juce::MouseEvent& event) override
    {
        if (onMouseDoubleClick)
            onMouseDoubleClick (event);
    }

    std::function<void(const juce::MouseEvent&)> onMouseDown;
    std::function<void(const juce::MouseEvent&)> onMouseUp;
    std::function<void(const juce::MouseEvent&)> onMouseDoubleClick;

private:
    juce::Component::SafePointer<juce::Component> listenedComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MouseLambdas)
};

}
