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

namespace foleys
{

ToolBox::ToolBox (juce::Component* parentToUse)
  : parent (parentToUse)
{
    setOpaque (true);
    setBounds (100, 100, 300, 700);
    addToDesktop (getLookAndFeel().getMenuWindowFlags());

    startTimer (100);

    setVisible (true);
}

void ToolBox::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
    g.setColour (juce::Colours::silver);
    g.drawRect (getLocalBounds().toFloat(), 2.0f);
    g.drawFittedText ("foleys GUI Magic", getLocalBounds().withHeight (24), juce::Justification::centred, 1);
}

void ToolBox::timerCallback ()
{
    if (parent == nullptr)
        return;

    const auto pos = parent->getScreenPosition();
    const auto height = parent->getHeight();
    if (pos != parentPos || height != parentHeight)
    {
        const auto width = 200;
        parentPos = pos;
        parentHeight = height;
        setBounds (parentPos.getX() - width, parentPos.getY(),
                   width,                    parentHeight * 0.9f);
    }
}



} // namespace foleys
