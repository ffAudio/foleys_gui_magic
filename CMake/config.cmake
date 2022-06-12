#[[

CMake package configuration file for foleys_gui_magic

This file is loaded by the find_package command to load an installed copy of foleys_gui_magic
and bring it into another CMake build tree.

The way our install works is that we replicate the layout of the source tree in the package root, and 
in this file we call juce_add_module just the same way that building this project does.

]]

@PACKAGE_INIT@

check_required_components ("@PROJECT_NAME@")

#

include (FeatureSummary)
include (FindPackageMessage)

set_package_properties ("${CMAKE_FIND_PACKAGE_NAME}" 
						PROPERTIES 
							URL "https://foleysfinest.com/developer/pluginguimagic/"
                        	DESCRIPTION "A GUI builder module for JUCE")

#

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include (CMakeFindDependencyMacro)

find_dependency (JUCE 6)

juce_add_module ("${CMAKE_CURRENT_LIST_DIR}"
                 ALIAS_NAMESPACE Foleys)

find_package_message ("${CMAKE_FIND_PACKAGE_NAME}" 
					  "foleys_gui_magic - local install"
                      "foleys_gui_magic [${CMAKE_CURRENT_LIST_DIR}]")
