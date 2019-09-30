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

GuiTreeItem::~GuiTreeItem()
{
    tree.getParent().removeChild (tree, nullptr);
    clearSubItems();
}

void foleys::GuiTreeItem::paintItem (juce::Graphics& g, int width, int height)
{
    if (isSelected())
        g.fillAll (juce::Colours::blue.withAlpha (0.3f));

    g.setColour (juce::Colours::black);
    g.setFont (height * 0.7f);

    juce::String s = tree.getType().toString(); // tree.getProperty("caption").toString();
    if (s.isEmpty()) s = "Root";

    g.drawText (s, 4, 0, width - 4, height, juce::Justification::centredLeft, true);
}

void GuiTreeItem::itemOpennessChanged (bool isNowOpen)
{
    if (isNowOpen)
    {
        if (getNumSubItems() == 0)
        {
            for (const auto& child : tree)
            {
                // this leaks, need to find out why!
                addSubItem (new GuiTreeItem (child));
            }
        }
    }
}

void GuiTreeItem::itemSelectionChanged (bool isNowSelected)
{
    if (isNowSelected)
    {
        // update right panel to show the new selectedItem's properties!
        DBG (tree.toXmlString());
    }
}

}
