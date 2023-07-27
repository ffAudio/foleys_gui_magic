/*
 ==============================================================================
    Copyright (c) 2020-2023 Foleys Finest Audio - Daniel Walz
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

#include <juce_cryptography/juce_cryptography.h>

#include "foleys_ApplicationSettings.h"
#include "../Helpers/foleys_ScopedInterProcessLock.h"

namespace foleys
{

ApplicationSettings::ApplicationSettings()
{
    settings.addListener (this);
}

ApplicationSettings::~ApplicationSettings()
{
    stopTimer();
    settings.removeListener (this);
}

void ApplicationSettings::setFileName (juce::File file)
{
    if (file == settingsFile)
        return;

    settingsFile = file;
    startTimerHz (1);
}

void ApplicationSettings::load()
{
    ScopedInterProcessLock lock (settingsFile.getFileName() + ".lock", 500,
    [this]
    {
        auto newChecksum = juce::MD5 (settingsFile).toHexString();
        if (checksum == newChecksum)
            return;

        auto stream = settingsFile.createInputStream();
        if (stream.get() == nullptr)
            return;

        auto tree = juce::ValueTree::fromXml (stream->readEntireStreamAsString());
        if (! tree.isValid())
            return;

        settings.copyPropertiesAndChildrenFrom (tree, nullptr);
        settings.addListener (this);

        checksum = newChecksum;
        sendChangeMessage();
    });
}

void ApplicationSettings::save()
{
    ScopedInterProcessLock lock (settingsFile.getFileName() + ".lock", 1000,
    [this] {
        auto parent = settingsFile.getParentDirectory();
        parent.createDirectory();

        auto stream = settingsFile.createOutputStream();
        if (stream.get() == nullptr)
            return;

        stream->setPosition (0);
        stream->truncate();
        stream->writeString (settings.toXmlString());

        checksum = juce::MD5 (settingsFile).toHexString();
    });
}

void ApplicationSettings::timerCallback()
{
    load();
}

void ApplicationSettings::valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&)
{
    save();
}

void ApplicationSettings::valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int)
{
    save();
}

void ApplicationSettings::valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&)
{
    save();
}

}

