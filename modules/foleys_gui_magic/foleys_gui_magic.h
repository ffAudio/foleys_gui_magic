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

    ID:            foleys_gui_magic
    vendor:        Foleys Finest Audio
    version:       1.3.9
    name:          Foleys GUI magic
    description:   This module allows to create GUI with a drag and drop editor
    dependencies:  juce_core, juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_utils, juce_audio_processors, juce_gui_basics, juce_dsp,
                   juce_cryptography

    website:       https://foleysfinest.com/
    license:       BSD V2 3-clause

    END_JUCE_MODULE_DECLARATION

 ==============================================================================
 */

#pragma once

/** Config: FOLEYS_SHOW_GUI_EDITOR_PALLETTE
            Enables the GUI editor palette allowing to edit the GUI. Ideally set this to 0 in a release build.
  */
#ifndef FOLEYS_SHOW_GUI_EDITOR_PALLETTE
#define FOLEYS_SHOW_GUI_EDITOR_PALLETTE 1
#endif

/** Config: FOLEYS_ENABLE_BINARY_DATA
            Makes the binary resources available to the GUI. Make sure you actually have
            at least one file added, or this will fail to compile.
 */
#ifndef FOLEYS_ENABLE_BINARY_DATA
#define FOLEYS_ENABLE_BINARY_DATA 0
#endif

/** Config: FOLEYS_ENABLE_OPEN_GL_CONTEXT
            If selected an juce OpenGLCOntext is attached. Not a big difference on OSX, but vital on Windows.
  */
#ifndef FOLEYS_ENABLE_OPEN_GL_CONTEXT
#define FOLEYS_ENABLE_OPEN_GL_CONTEXT 1
#endif


#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_cryptography/juce_cryptography.h>
#include <juce_dsp/juce_dsp.h>

#if JUCE_MODULE_AVAILABLE_juce_opengl && FOLEYS_ENABLE_OPEN_GL_CONTEXT
#include <juce_opengl/juce_opengl.h>
#endif

#if JUCE_MODULE_AVAILABLE_juce_gui_extra
#include <juce_gui_extra/juce_gui_extra.h>
#endif

#include "General/foleys_StringDefinitions.h"
#include "General/foleys_ApplicationSettings.h"
#include "General/foleys_SettableProperties.h"
#include "General/foleys_Resources.h"

#include "Helpers/foleys_ScopedInterProcessLock.h"
#include "Helpers/foleys_PopupMenuHelper.h"
#include "Helpers/foleys_MouseLambdas.h"
#include "Helpers/foleys_ParameterAttachment.h"
#include "Helpers/foleys_AtomicValueAttachment.h"
#include "Helpers/foleys_Conversions.h"
#include "Helpers/foleys_DefaultGuiTrees.h"

#include "Layout/foleys_GradientBackground.h"
#include "Layout/foleys_BoxModel.h"
#include "Layout/foleys_Stylesheet.h"
#include "Layout/foleys_Decorator.h"
#include "Layout/foleys_GuiItem.h"
#include "Layout/foleys_Container.h"
#include "Layout/foleys_RootItem.h"

#include "LookAndFeels/foleys_JuceLookAndFeels.h"
#include "LookAndFeels/foleys_LookAndFeel.h"
#include "LookAndFeels/foleys_Skeuomorphic.h"

#include "Visualisers/foleys_MagicLevelSource.h"
#include "Visualisers/foleys_MagicPlotSource.h"
#include "Visualisers/foleys_MagicFilterPlot.h"
#include "Visualisers/foleys_MagicAnalyser.h"
#include "Visualisers/foleys_MagicOscilloscope.h"

#include "Widgets/foleys_AutoOrientationSlider.h"
#include "Widgets/foleys_MagicLevelMeter.h"
#include "Widgets/foleys_MagicPlotComponent.h"
#include "Widgets/foleys_XYDragComponent.h"
#include "Widgets/foleys_FileBrowserDialog.h"
#include "Widgets/foleys_MidiLearnComponent.h"
#include "Widgets/foleys_MidiDrumpadComponent.h"

#include "State/foleys_RadioButtonManager.h"
#include "State/foleys_ParameterManager.h"
#include "State/foleys_MidiParameterMapper.h"
#include "State/foleys_MagicGUIState.h"
#include "State/foleys_MagicProcessorState.h"

#include "General/foleys_MagicGUIBuilder.h"
#include "General/foleys_MagicPluginEditor.h"
#include "General/foleys_MagicProcessor.h"
