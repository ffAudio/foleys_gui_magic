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

#include "foleys_Resources.h"

namespace BinaryDataFallbacks {
const int namedResourceListSize = 0;
const char** namedResourceList = nullptr;
const char* getNamedResource(const char*, int&) { return nullptr; }
}  // namespace BinaryDataFallbacks

namespace BinaryData {
using namespace BinaryDataFallbacks;
}

namespace foleys
{

juce::StringArray Resources::getResourceFileNames()
{
    juce::StringArray names;

    for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
        names.add (juce::String::fromUTF8 (BinaryData::namedResourceList [i]));

    return names;
}

juce::Image Resources::getImage (const juce::String& name)
{
    int dataSize = 0;
    const char* data = BinaryData::getNamedResource (name.toRawUTF8(), dataSize);
    if (data != nullptr)
        return juce::ImageCache::getFromMemory (data, dataSize);

    return {};
}

}
