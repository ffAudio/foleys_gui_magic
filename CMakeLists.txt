cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set (version 1.4.0)

project ("foleys_gui_magic"
        DESCRIPTION "PluginGuiMagic"
        HOMEPAGE_URL "https://foleysfinest.com/PluginGuiMagic"
        LANGUAGES CXX)

option (FOLEYS_BUILD_TESTS "Build and run the unit tests" ON)
option (FOLEYS_BUILD_EXAMPLES "Build the examples" ON)
option (FOLEYS_RUN_PLUGINVAL "Run pluginval on the examples" ON)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)
set_property (GLOBAL PROPERTY DEBUG_CONFIGURATIONS Debug)
set_property (DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${CMAKE_CURRENT_LIST_DIR}/logs")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_CXX_VISIBILITY_PRESET hidden)
set (CMAKE_VISIBILITY_INLINES_HIDDEN ON)

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

if (FOLEYS_BUILD_TESTS)
    message(Tests are TODO)
endif ()

if (FOLEYS_BUILD_EXAMPLES)
    message(Build examples)
    add_subdirectory(Examples)
endif ()

if (FOLEYS_RUN_PLUGINVAL)
    include (CMakeIncludes/Pluginval.cmake)
endif ()

project (foleys_gui_magic)
