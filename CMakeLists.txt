cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

#
# CPM.cmake configuration

include (CMake/CPM.cmake)

#
# Add JUCE

CPMAddPackage (NAME JUCE
        GITHUB_REPOSITORY juce-framework/JUCE
        GIT_TAG origin/develop
        OPTIONS "JUCE_BUILD_EXAMPLES OFF" "JUCE_BUILD_EXTRAS OFF" "JUCE_ENABLE_MODULE_SOURCE_GROUPS ON")

project (foleys_gui_magic_tests)
