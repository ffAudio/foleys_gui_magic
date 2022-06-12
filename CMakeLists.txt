cmake_minimum_required (VERSION 3.15 FATAL_ERROR)

set_property (GLOBAL PROPERTY USE_FOLDERS YES)

project (FoleysGUIMagic
         VERSION 1.3.7
         DESCRIPTION "A GUI builder module for JUCE"
         HOMEPAGE_URL "https://foleysfinest.com/developer/pluginguimagic/"
         LANGUAGES CXX)

enable_testing()

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

find_package (JUCE 6 REQUIRED)

include (GNUInstallDirs)

juce_add_module ("${CMAKE_CURRENT_LIST_DIR}/foleys_gui_magic"
                 ALIAS_NAMESPACE Foleys)

# tests

option (FGM_BUILD_TESTS
        "Build the foleys_gui_magic unit tests"
        "${PROJECT_IS_TOP_LEVEL}")

if(FGM_BUILD_TESTS)
        add_subdirectory (Tests)
endif()

# install rules

set (FGM_INSTALL_DEST "${CMAKE_INSTALL_LIBDIR}/cmake/foleys_gui_magic"
        CACHE STRING
        "Directory below INSTALL_PREFIX where the foleys_gui_magic CMake package files will be installed to")

install (DIRECTORY CMake
         DESTINATION "${FGM_INSTALL_DEST}"
         COMPONENT foleys_gui_magic)

install (DIRECTORY foleys_gui_magic
         DESTINATION "${FGM_INSTALL_DEST}"
         COMPONENT foleys_gui_magic)

include (CMakePackageConfigHelpers)

write_basic_package_version_file (foleys_gui_magic-config-version.cmake 
                                  VERSION "${PROJECT_VERSION}"
                                  COMPATIBILITY SameMajorVersion)

configure_package_config_file (CMake/config.cmake foleys_gui_magic-config.cmake
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
