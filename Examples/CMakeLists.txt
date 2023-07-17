# ==============================================================================
# Copyright (c) 2023 Foleys Finest Audio - Daniel Walz
# All rights reserved.
#
# **BSD 3-Clause License**
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# ==============================================================================
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
# ==============================================================================

# Add examples

message (STATUS "Build examples")
message (STATUS "Run pluginval on examples: ${FOLEYS_RUN_PLUGINVAL}")

if(FOLEYS_RUN_PLUGINVAL)
    include (PGMPluginval)
endif()

# for our internal use only
# this function only adds pluginval tests if FOLEYS_RUN_PLUGINVAL is on, and uses our test naming convention
function(__pgm_internal_add_pluginval_tests pluginTarget)
    if(FOLEYS_RUN_PLUGINVAL)
        pgm_add_pluginval_tests ("${pluginTarget}" 
            TEST_BASE_NAME "foleys_gui_magic.examples.${pluginTarget}.pluginval")
    endif()
endfunction()


# check which formats we want to build
set(FORMATS "VST3" "Standalone")
if(APPLE OR WIN32) # no AAX on Linux
#    raises error: Use juce_set_aax_sdk_path to set up the AAX sdk before adding AAX targets
#    list (APPEND FORMATS AAX)
endif()

if (VST2_PATH)
    list(APPEND FORMATS "VST2")
    juce_set_vst2_sdk_path(${VST2_PATH})
endif()

if (APPLE)
    list(APPEND FORMATS "AU")
endif()

if (LINUX)
    list(APPEND FORMATS "LV2")
endif()

# setup the copying to the output folder
if (APPLE)
    set(COPY_FOLDER ${CMAKE_SOURCE_DIR}/Builds/MacOSX)
elseif(WIN32)
    set(COPY_FOLDER ${CMAKE_SOURCE_DIR}/Builds/VisualStudio2019)
endif()

add_subdirectory(APVTS_Tutorial)
add_subdirectory(EqualizerExample)
add_subdirectory(ExtendingExample)
add_subdirectory(FoleysSynth)
add_subdirectory(SignalGenerator)
add_subdirectory(Vanilla)
add_subdirectory(PlayerExample)
