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

ToolBox::ToolBox (juce::Component* parentToUse, MagicBuilder& builderToControl)
  : parent (parentToUse),
    builder (builderToControl),
    treeEditor (builderToControl),
    propertiesEditor (builderToControl)
{
    setOpaque (true);

    saveXml.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);
    loadXml.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);
    saveCSS.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);
    loadCSS.setConnectedEdges (juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);

    addAndMakeVisible (saveXml);
    addAndMakeVisible (loadXml);
    addAndMakeVisible (saveCSS);
    addAndMakeVisible (loadCSS);

    saveXml.onClick = [&]
    {
        juce::FileChooser myChooser ("Save XML to file...", getLastLocation(), "*.xml");
        if (myChooser.browseForFileToSave (true))
        {
            juce::File xmlFile (myChooser.getResult());
            juce::FileOutputStream stream (xmlFile);
            stream.setPosition (0);
            stream.truncate();
            stream.writeString (builder.getGuiTree().toXmlString());
            lastLocation = xmlFile;
        }
    };

    loadXml.onClick = [&]
    {
        juce::FileChooser myChooser ("Load from XML file...", getLastLocation(), "*.xml");
        if (myChooser.browseForFileToOpen())
        {
            juce::File xmlFile (myChooser.getResult());
            juce::FileInputStream stream (xmlFile);
            auto tree = juce::ValueTree::fromXml (stream.readEntireStreamAsString());

            if (tree.isValid() && tree.getType() == IDs::magic)
            {
                builder.restoreGUI (tree);
                stateWasReloaded();
            }

            lastLocation = xmlFile;
        }
    };

    treeEditor.onSelectionChanged = [&] (juce::ValueTree& ref)
    {
        propertiesEditor.setNodeToEdit (ref);
    };

    addAndMakeVisible (treeEditor);
    addAndMakeVisible (resizer);
    addAndMakeVisible (propertiesEditor);

    resizeManager.setItemLayout (0, 1, -1.0, -0.6);
    resizeManager.setItemLayout (1, 6, 6, 6);
    resizeManager.setItemLayout (2, 1, -1.0, -0.4);

    setBounds (100, 100, 300, 700);
    addToDesktop (getLookAndFeel().getMenuWindowFlags());

    startTimer (100);

    setVisible (true);

    stateWasReloaded();
}

ToolBox::~ToolBox()
{
}

void ToolBox::stateWasReloaded()
{
    treeEditor.setValueTree (builder.getGuiTree());
    propertiesEditor.setStyle (builder.getStylesheet().getCurrentStyle());
}

void ToolBox::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
    g.setColour (juce::Colours::silver);
    g.drawRect (getLocalBounds().toFloat(), 2.0f);
    g.setColour (juce::Colours::white);
    g.drawFittedText ("foleys GUI Magic", getLocalBounds().withHeight (24), juce::Justification::centred, 1);
}

void ToolBox::resized()
{
    auto bounds = getLocalBounds().reduced (2).withTop (24);
    auto buttons = bounds.removeFromTop (24);
    auto w = buttons.getWidth() / 4;
    saveXml.setBounds (buttons.removeFromLeft (w));
    loadXml.setBounds (buttons.removeFromLeft (w));
    saveCSS.setBounds (buttons.removeFromLeft (w));
    loadCSS.setBounds (buttons);

    juce::Component* comps[] = { &treeEditor, &resizer, &propertiesEditor };
    resizeManager.layOutComponents (comps, 3,
                                    bounds.getX(),
                                    bounds.getY(),
                                    bounds.getWidth(),
                                    bounds.getHeight(),
                                    true, true);
}

void ToolBox::timerCallback ()
{
    if (parent == nullptr)
        return;

    const auto pos = parent->getScreenPosition();
    const auto height = parent->getHeight();
    if (pos != parentPos || height != parentHeight)
    {
        const auto width = 260;
        parentPos = pos;
        parentHeight = height;
        setBounds (parentPos.getX() - width, parentPos.getY(),
                   width,                    parentHeight * 0.9f);
    }
}

juce::File ToolBox::getLastLocation() const
{
    if (lastLocation.exists())
        return lastLocation;

    auto start = juce::File::getSpecialLocation (juce::File::currentExecutableFile);
    while (start.exists() && start.getFileName() != "Builds")
        start = start.getParentDirectory();

    if (start.getFileName() == "Builds")
    {
        auto resources = start.getSiblingFile ("Resources");
        if (resources.isDirectory())
            return resources;

        auto sources = start.getSiblingFile ("Sources");
        if (sources.isDirectory())
            return sources;
    }

    return juce::File::getSpecialLocation (juce::File::currentExecutableFile);
}


} // namespace foleys
