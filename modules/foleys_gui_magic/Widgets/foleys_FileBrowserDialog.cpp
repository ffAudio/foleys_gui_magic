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

#include "foleys_FileBrowserDialog.h"

namespace foleys
{

FileBrowserDialog::FileBrowserDialog (const juce::String cancelText,
                                      const juce::String acceptText,
                                      int                browserMode,
                                      const juce::File&  directory,
                                      std::unique_ptr<juce::FileFilter> filter)
  : fileFilter (std::move (filter)),
    cancel (cancelText),
    accept (acceptText)
{
    addAndMakeVisible (cancel);
    addAndMakeVisible (accept);

    fileBrowser = std::make_unique<juce::FileBrowserComponent>(browserMode,
                                                               directory,
                                                               fileFilter.get(),
                                                               nullptr);
    addAndMakeVisible (fileBrowser.get());
    fileBrowser->addListener (this);
}

void FileBrowserDialog::resized()
{
    auto area = getLocalBounds().reduced (getWidth() / 10);

    auto buttons = area.removeFromBottom (26).reduced (20, 0);
    accept.setBounds (buttons.removeFromRight (100));
    buttons.removeFromRight (10);
    cancel.setBounds (buttons.removeFromRight (100));

    area.removeFromBottom (10);
    fileBrowser->setBounds (area);
}

void FileBrowserDialog::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black.withAlpha (0.9f));
}

void FileBrowserDialog::setCancelFunction (std::function<void()> func)
{
    cancel.onClick = std::move (func);
}

void FileBrowserDialog::setAcceptFunction (std::function<void()> func)
{
    accept.onClick = std::move (func);
}

juce::File FileBrowserDialog::getFile()
{
    return fileBrowser->getSelectedFile (0);
}

void FileBrowserDialog::fileDoubleClicked (const juce::File& file)
{
    juce::ignoreUnused (file);
    if (accept.onClick)
        accept.onClick();
}

} // namespace foleys
