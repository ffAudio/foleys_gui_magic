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

class Skeuomorphic : public juce::LookAndFeel_V4
{
public:
    Skeuomorphic() = default;

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider&) override;

private:
    struct knobImages
    {
        knobImages (juce::Image bg, juce::Image fg) : backgroundImage {bg}, foregroundImage {fg} {}
        knobImages (const knobImages& src) = default;
        knobImages (knobImages&& src) = default;

        juce::Image backgroundImage;
        juce::Image foregroundImage;
    };
    std::map<int, knobImages> knobsBgs;
    int knobsMaxSize {6};

    // hardcoded colors for the knobs
    const juce::Colour whiteA010 = juce::Colours::white.withAlpha (juce::uint8 ( 10));
    const juce::Colour whiteA042 = juce::Colours::white.withAlpha (juce::uint8 ( 42));
    const juce::Colour blackA042 = juce::Colours::black.withAlpha (juce::uint8 ( 42));
    const juce::Colour blackA092 = juce::Colours::black.withAlpha (juce::uint8 ( 92));
    const juce::Colour blackA122 = juce::Colours::black.withAlpha (juce::uint8 (122));
    const juce::Colour blackA142 = juce::Colours::black.withAlpha (juce::uint8 (142));

    const knobImages& getKnobImages (int diameter);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Skeuomorphic)
};

} // namespace foleys
