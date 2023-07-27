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

#include <juce_core/juce_core.h>

namespace foleys
{

template<typename T>
struct Box
{
    T top    = {};
    T left   = {};
    T right  = {};
    T bottom = {};

    Box() = default;
    Box (T value) : top (value), left (value), right (value), bottom (value) {}

    static Box fromString (const juce::String& text)
    {
        Box b;
        auto values = juce::StringArray::fromTokens (text, ", ", "\"");
        values.removeEmptyStrings();

        switch (values.size())
        {
            case 1:
                b.top = T (values [0].getFloatValue());
                b.left = b.top;
                b.right = b.top;
                b.bottom = b.top;
                break;
            case 2:
                b.top = T (values [0].getFloatValue());
                b.left = T (values [1].getFloatValue());
                b.right = b.left;
                b.bottom = b.top;
                break;
            case 3:
                b.top = T (values [0].getFloatValue());
                b.left = T (values [1].getFloatValue());
                b.bottom = T (values [2].getFloatValue());
                b.right = b.left;
                break;
            case 4:
                b.top = T (values [0].getFloatValue());
                b.right = T (values [1].getFloatValue());
                b.bottom = T (values [2].getFloatValue());
                b.left = T (values [3].getFloatValue());
                break;
            default:
                break;
        }

        return b;
    }

    juce::Rectangle<T> reducedRect (juce::Rectangle<T> rect) const
    {
        return rect.withTrimmedTop (top)
                   .withTrimmedLeft (left)
                   .withTrimmedRight (right)
                   .withTrimmedBottom (bottom);
    }
};

}
