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

cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set (FGM_VERSION 1.4.1)

project (foleys_gui_magic
        DESCRIPTION "PluginGuiMagic"
        HOMEPAGE_URL "https://foleysfinest.com/PluginGuiMagic"
        LANGUAGES CXX)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeIncludes")

include (PGMPluginval)

option (FOLEYS_BUILD_EXAMPLES "Build the examples" ON)
option (FOLEYS_BUILD_TESTS "Build and run the unit tests" ON)
option (FOLEYS_RUN_PLUGINVAL "Run pluginval on the example plugins" ON)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)
set_property (GLOBAL PROPERTY DEBUG_CONFIGURATIONS Debug)
set_property (DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${CMAKE_CURRENT_LIST_DIR}/logs")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_CXX_VISIBILITY_PRESET hidden)
set (CMAKE_VISIBILITY_INLINES_HIDDEN ON)

set (BUILD_SHARED_LIBS OFF)

# universal binaries on Mac
set (CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING
        "Architectures to build on MacOS. Set to arm64\;x86_64 to build universal (fat) binaries, or just one of those for faster build times.")

# static linking on Windows
set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# JUCE
include (FetchContent)

FetchContent_Declare (
        juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG origin/master
        GIT_SHALLOW ON)

set (JUCE_ENABLE_MODULE_SOURCE_GROUPS ON)
set (JUCE_BUILD_EXTRAS OFF)
set (JUCE_BUILD_EXAMPLES OFF)

FetchContent_MakeAvailable (juce)

add_subdirectory (modules)

if(FOLEYS_BUILD_TESTS)
    enable_testing()
    add_subdirectory (Tests)

    if(foleys_gui_magic_IS_TOP_LEVEL)
        include (CTest)
    endif()
endif()

# this must be AFTER the enable_testing() call above!
if (FOLEYS_BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif ()

# install rules

set (FGM_INSTALL_DEST "${CMAKE_INSTALL_LIBDIR}/cmake/foleys_gui_magic"
        CACHE STRING
        "Directory below INSTALL_PREFIX where the foleys_gui_magic CMake package files will be installed to")

install (DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
        DESTINATION "${FGM_INSTALL_DEST}/.."
        COMPONENT foleys_gui_magic
        PATTERN Tests/* EXCLUDE
        PATTERN *.md EXCLUDE
        PATTERN .git/* EXCLUDE
        PATTERN .github/* EXCLUDE
        PATTERN *.json EXCLUDE
        PATTERN CMakeLists.txt EXCLUDE
        PATTERN "${CMAKE_CURRENT_BINARY_DIR}/" EXCLUDE)

include (CMakePackageConfigHelpers)

#write_basic_package_version_file (foleys_gui_magic-config-version.cmake
#        VERSION "${FGM_VERSION}"
#        COMPATIBILITY SameMajorVersion
#        ARCH_INDEPENDENT)

configure_package_config_file (CMakeIncludes/config.cmake foleys_gui_magic-config.cmake
        INSTALL_DESTINATION "${FGM_INSTALL_DEST}"
        NO_SET_AND_CHECK_MACRO)

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/foleys_gui_magic-config-version.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/foleys_gui_magic-config.cmake"
        DESTINATION "${FGM_INSTALL_DEST}"
        COMPONENT foleys_gui_magic)

include (CPackComponent)

cpack_add_component (foleys_gui_magic
        GROUP Foleys
        INSTALL_TYPES Developer)

export (PACKAGE foleys_gui_magic)

