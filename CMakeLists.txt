cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set (FGM_VERSION 1.4.0)

project (foleys_gui_magic
        DESCRIPTION "PluginGuiMagic"
        HOMEPAGE_URL "https://foleysfinest.com/PluginGuiMagic"
        LANGUAGES CXX)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeIncludes")

include (PGMPluginval)

option (FOLEYS_BUILD_TESTS "Build and run the unit tests" ON)
option (FOLEYS_BUILD_EXAMPLES "Build the examples" ON)

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

if (FOLEYS_BUILD_EXAMPLES)
    option (FOLEYS_RUN_PLUGINVAL "Run pluginval on the examples" ON)
    message(Build examples)
    add_subdirectory(Examples)
endif ()

if(FOLEYS_BUILD_TESTS)
    enable_testing()
    add_subdirectory (Tests)

    if(foleys_gui_magic_IS_TOP_LEVEL)
        include (CTest)
    endif()
endif()

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

