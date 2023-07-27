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


namespace foleys
{

class JuceLookAndFeel_V2 : public juce::LookAndFeel_V2
{
public:
    JuceLookAndFeel_V2() = default;

    void drawPopupMenuItemWithOptions (juce::Graphics&, const juce::Rectangle<int>& area,
                                       bool isHighlighted,
                                       const juce::PopupMenu::Item& item,
                                       const juce::PopupMenu::Options&) override;

private:
    juce::Colour findPopupColour (int colourId, juce::Component* target);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceLookAndFeel_V2)
};

//==============================================================================

class JuceLookAndFeel_V3 : public juce::LookAndFeel_V3
{
public:
    JuceLookAndFeel_V3() = default;

    void drawPopupMenuItemWithOptions (juce::Graphics&, const juce::Rectangle<int>& area,
                                       bool isHighlighted,
                                       const juce::PopupMenu::Item& item,
                                       const juce::PopupMenu::Options&) override;

private:
    juce::Colour findPopupColour (int colourId, juce::Component* target);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceLookAndFeel_V3)
};

//==============================================================================

class JuceLookAndFeel_V4 : public juce::LookAndFeel_V4
{
public:
    JuceLookAndFeel_V4() = default;

    void drawPopupMenuItemWithOptions (juce::Graphics&, const juce::Rectangle<int>& area,
                                       bool isHighlighted,
                                       const juce::PopupMenu::Item& item,
                                       const juce::PopupMenu::Options&) override;

private:
    juce::Colour findPopupColour (int colourId, juce::Component* target);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceLookAndFeel_V4)
};

} // namespace foleys
