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
