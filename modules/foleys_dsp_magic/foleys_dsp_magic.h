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

    BEGIN_JUCE_MODULE_DECLARATION

    ID:            foleys_dsp_magic
    vendor:        Foleys Finest Audio
    version:       1.3.9
    name:          Foleys GUI magic
    description:   This module allows to create DSP programs from XML
    dependencies:  foleys_gui_magic

    website:       https://foleysfinest.com/
    license:       BSD V2 3-clause

    END_JUCE_MODULE_DECLARATION

 ==============================================================================
 */

#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_dsp/juce_dsp.h>

#include "DSP/foleys_Input.h"
#include "DSP/foleys_Output.h"
#include "DSP/foleys_DspNode.h"
#include "DSP/foleys_DspProgram.h"
#include "DSP/foleys_MagicDspBuilder.h"
#include "Nodes/foleys_BuiltinNodes.h"
#include "Nodes/foleys_Gain.h"
#include "Nodes/foleys_Multiplier.h"
#include "Nodes/foleys_Summing.h"
#include "Nodes/foleys_Analyser.h"
#include "Nodes/foleys_Biquad.h"
#include "Nodes/foleys_Noise.h"
#include "Nodes/foleys_Oscillator.h"
#include "Nodes/foleys_Oscilloscope.h"
#include "Processor/foleys_PluginParameters.h"
#include "Processor/foleys_PluginProcessor.h"

