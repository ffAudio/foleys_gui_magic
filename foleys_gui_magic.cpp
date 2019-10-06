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

#include "foleys_gui_magic.h"

#include <stack>

#include "General/foleys_StringDefinitions.cpp"

#include "General/foleys_MagicGUIBuilder.cpp"
#include "General/foleys_MagicPluginEditor.cpp"
#include "General/foleys_MagicProcessorState.cpp"
#include "General/foleys_JuceFactories.cpp"

#include "Layout/foleys_Stylesheet.cpp"
#include "Layout/foleys_Decorator.cpp"
#include "Layout/foleys_Container.cpp"

#include "Visualisers/foleys_MagicFilterPlot.cpp"
#include "Visualisers/foleys_MagicAnalyser.cpp"
#include "Visualisers/foleys_MagicOscilloscope.cpp"
#include "Visualisers/foleys_MagicPlotComponent.cpp"

#include "LookAndFeels/foleys_LookAndFeel.cpp"

#if FOLEYS_SHOW_GUI_EDITOR_PALLETTE

#include "Editor/foleys_ToolBox.cpp"
#include "Editor/foleys_GUITreeEditor.cpp"
#include "Editor/foleys_PropertiesEditor.cpp"

#endif // FOLEYS_SHOW_GUI_EDITOR_PALLETTE
