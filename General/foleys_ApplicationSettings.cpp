/*
 ==============================================================================
    Copyright (c) 2020 Foleys Finest Audio - Daniel Walz
    All rights reserved.

    License for non-commercial projects:

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

    License for commercial products:

    To sell commercial products containing this module, you are required to buy a
    License from https://foleysfinest.com/developer/pluginguimagic/

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

ApplicationSettings::ApplicationSettings()
{
    settings.addListener (this);
}

ApplicationSettings::~ApplicationSettings()
{
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
    juce::InterProcessLock lock (settingsFile.getFileName() + ".lock");

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
}

void ApplicationSettings::save()
{
    juce::InterProcessLock lock (settingsFile.getFileName() + ".lock");

    auto parent = settingsFile.getParentDirectory();
    parent.createDirectory();

    auto stream = settingsFile.createOutputStream();
    if (stream.get() == nullptr)
        return;

    stream->setPosition (0);
    stream->truncate();
    stream->writeString (settings.toXmlString());

    checksum = juce::MD5 (settingsFile).toHexString();
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

