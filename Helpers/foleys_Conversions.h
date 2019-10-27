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

namespace Conversions
{

    template<typename FloatType>
    static inline juce::NormalisableRange<FloatType> makeLogarithmicRange (FloatType min, FloatType max)
    {
        return juce::NormalisableRange<FloatType>
        (
            min, max,
            [](FloatType start, FloatType end, FloatType normalised)
            {
                return start + (std::pow (FloatType (2), normalised * FloatType (10)) - FloatType (1)) * (end - start) / FloatType (1023);
            },
            [](FloatType start, FloatType end, FloatType value)
            {
                return (std::log (((value - start) * FloatType (1023) / (end - start)) + FloatType (1)) / std::log (FloatType (2))) / FloatType (10);
            },
            [](FloatType start, FloatType end, FloatType value)
            {
                // optimised for frequencies: >3 kHz: 2 decimals
                if (value > FloatType (3000))
                    return juce::jlimit (start, end, FloatType (100) * juce::roundToInt (value / FloatType (100)));

                // optimised for frequencies: 1-3 kHz: 1 decimal
                if (value > FloatType (1000))
                    return juce::jlimit (start, end, FloatType (10) * juce::roundToInt (value / FloatType (10)));

                return juce::jlimit (start, end, FloatType (juce::roundToInt (value)));
            });
    }

}

} // namespace foleys
