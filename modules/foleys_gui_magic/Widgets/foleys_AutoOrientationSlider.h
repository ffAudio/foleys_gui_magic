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

#include <juce_gui_basics/juce_gui_basics.h>

namespace foleys
{

/**
 This is a Slider, that holds an attachment to the AudioProcessorValueTreeState
 */
class AutoOrientationSlider  : public juce::Slider
{
public:

    AutoOrientationSlider() = default;

    void setAutoOrientation (bool shouldAutoOrient)
    {
        autoOrientation = shouldAutoOrient;
        resized();
    }

    void paint (juce::Graphics& g) override
    {
        if (filmStrip.isNull() || numImages == 0)
        {
            juce::Slider::paint (g);
        }
        else
        {
            auto index = juce::roundToInt ((numImages - 1) * valueToProportionOfLength (getValue()));
            auto knobArea = getLookAndFeel().getSliderLayout(*this).sliderBounds;

            if (horizontalFilmStrip)
            {
                auto w = filmStrip.getWidth() / numImages;
                g.drawImage (filmStrip, knobArea.getX(), knobArea.getY(), knobArea.getWidth(), knobArea.getHeight(),
                             index * w, 0, w, filmStrip.getHeight());
            }
            else
            {
                auto h = filmStrip.getHeight() / numImages;
                g.drawImage (filmStrip, knobArea.getX(), knobArea.getY(), knobArea.getWidth(), knobArea.getHeight(),
                             0, index * h, filmStrip.getWidth(), h);
            }
        }
    }

    void resized() override
    {
        if (autoOrientation)
        {
            const auto w = getWidth();
            const auto h = getHeight();

            if (w > 2 * h)
                setSliderStyle (juce::Slider::LinearHorizontal);
            else if (h > 2 * w)
                setSliderStyle (juce::Slider::LinearVertical);
            else
                setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        }

        juce::Slider::resized();
    }

    void setFilmStrip (juce::Image& image)
    {
        filmStrip = image;
    }

    void setNumImages (int num, bool horizontal)
    {
        numImages = num;
        horizontalFilmStrip = horizontal;
    }

private:

    bool autoOrientation = true;

    juce::Image filmStrip;
    int         numImages = 0;
    bool        horizontalFilmStrip = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoOrientationSlider)
};

} // namespace foleys
