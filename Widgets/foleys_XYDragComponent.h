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

#pragma once

namespace foleys
{

/**
 This is a 2D parameter dragging component.
 */
class XYDragComponent  : public juce::Component
{
public:

    enum ColourIds
    {
        xyDotColourId = 0x2002000,
        xyDotOverColourId,
        xyHorizontalColourId,
        xyHorizontalOverColourId,
        xyVerticalColourId,
        xyVerticalOverColourId
    };

    XYDragComponent();

    void setCrossHair (bool horizontal, bool vertical);

    void paint (juce::Graphics& g) override;

    void setParameterX (juce::RangedAudioParameter* parameter);
    void setParameterY (juce::RangedAudioParameter* parameter);

    void setRightClickParameter (juce::RangedAudioParameter* parameter);

    void setRadius (float radius);
    void setSenseFactor (float factor);

    bool hitTest (int x, int y) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseMove (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseEnter (const juce::MouseEvent&) override;
    void mouseExit (const juce::MouseEvent&) override;

private:

    void updateWhichToDrag (juce::Point<float> p);

    int getXposition() const;
    int getYposition() const;

    bool mouseOverDot = false;
    bool mouseOverX   = false;
    bool mouseOverY   = false;

    bool wantsHorizontalDrag = true;
    bool wantsVerticalDrag = true;

    ParameterAttachment<float> xAttachment;
    ParameterAttachment<float> yAttachment;

    juce::RangedAudioParameter* contextMenuParameter = nullptr;

    float radius = 4.0f;
    float senseFactor = 2.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYDragComponent)
};

} // namespace foleys
