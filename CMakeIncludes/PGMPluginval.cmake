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

PGMPluginval
--------------

This module provides utility functions for setting up pluginval test cases.

This module searches for pluginval in your PATH; this module currently does not download or build 
pluginval if it can't be found, it will simply issue a warning and not create any test cases.

To help this module find pluginval, either make sure it can be found in your PATH (ie, check by
running `which pluginval` on Mac or `where pluginval` on Windows), or you can also explicitly set the
variable PLUGINVAL_PROGRAM to the full path to the pluginval executable.

]]

include_guard (GLOBAL)

find_program (PLUGINVAL_PROGRAM pluginval DOC "pluginval executable")

if (NOT PLUGINVAL_PROGRAM)
	message (WARNING "pluginval not found!")
endif ()

set (PLUGINVAL_STRICTNESS 5 CACHE STRING "pluginval strictness level, 1 to 10")

set_property (
	CACHE PLUGINVAL_STRICTNESS
	PROPERTY STRINGS
			 1 2 3 4 5 6 7 8 9 10)

set (PLUGINVAL_REPEATS 0 CACHE STRING "number of times to repeat pluginval tests. This can greatly increase test execution time.")

set (PLUGINVAL_SAMPLERATES "44100;44800;96000" CACHE STRING "list of samplerates to test in pluginval")

set (PLUGINVAL_BLOCKSIZES "1;250;512" CACHE STRING "list of blocksizes to test in pluginval")

set (PLUGINVAL_OUTPUT_DIR "${CMAKE_BINARY_DIR}/pluginval" CACHE PATH "Directory to write pluginval logs to")

mark_as_advanced (PLUGINVAL_PROGRAM PLUGINVAL_STRICTNESS PLUGINVAL_REPEATS PLUGINVAL_SAMPLERATES PLUGINVAL_BLOCKSIZES PLUGINVAL_OUTPUT_DIR)

#

#[[

  pgm_add_pluginval_tests (<pluginTarget>
  						  [TEST_BASE_NAME <name>])

This function registers CTest test cases to run pluginval on each format of the `pluginTarget`. 

Pluginval supports VST3, VST2, and AU (on Mac). Each one will be tested as a separate CTest test case.

On Mac, pluginval can only find audio units that are in the system AU directories, so the AU test
will only be added if the `JUCE_COPY_PLUGIN_AFTER_BUILD` option is enabled.

`pluginTarget` should be the name of your plugin's shared code target (the name you passed to
`juce_add_plugin`).

Example usages:

 juce_add_plugin (Foo ...)
 pgm_add_pluginval_tests (Foo)

 juce_add_plugin (Bar ...)
 pgm_add_pluginval_tests (Bar TEST_BASE_NAME BarTests)

]]
function (pgm_add_pluginval_tests pluginTarget)

	if (NOT TARGET "${pluginTarget}")
		message (
			FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} - plugin target ${pluginTarget} does not exist!")
	endif ()

	if (NOT PLUGINVAL_PROGRAM)
		return ()
	endif ()

	cmake_parse_arguments (ARG "" "TEST_BASE_NAME" "" ${ARGN})

	if(NOT ARG_TEST_BASE_NAME)
		set (ARG_TEST_BASE_NAME "${pluginTarget}.pluginval")
	endif()

	list (JOIN PLUGINVAL_SAMPLERATES "," sample_rates)
	list (JOIN PLUGINVAL_BLOCKSIZES "," block_sizes)

	get_target_property (format_targets "${pluginTarget}" JUCE_ACTIVE_PLUGIN_TARGETS)

	string (LENGTH "${pluginTarget}" pluginNameLen)
	math (EXPR startIdx "${pluginNameLen} + 1" OUTPUT_FORMAT DECIMAL)

	foreach (format_target IN LISTS format_targets)

		string (SUBSTRING "${format_target}" "${startIdx}" "-1" formatName)

		# pluginval can't find AUs unless they're copied to system folders
		if ("${formatName}" STREQUAL "AU" AND NOT JUCE_COPY_PLUGIN_AFTER_BUILD)
			continue ()
		endif ()

		if ("${formatName}" STREQUAL "Standalone"
		 OR "${formatName}" STREQUAL "AAX")
			continue ()
		endif ()

		get_target_property (plugin_artefact "${format_target}" JUCE_PLUGIN_ARTEFACT_FILE)

		add_test (
			NAME "${ARG_TEST_BASE_NAME}.${formatName}"
			COMMAND "${PLUGINVAL_PROGRAM}"
						--strictness-level "${PLUGINVAL_STRICTNESS}"
						--sample-rates "${sample_rates}"
						--block-sizes "${block_sizes}"
						--repeat "${PLUGINVAL_REPEATS}"
						--validate "${plugin_artefact}"
						--output-dir "${PLUGINVAL_OUTPUT_DIR}"
						--randomise --verbose)

		message (VERBOSE "Added pluginval test for plugin target ${format_target}")

	endforeach ()

endfunction ()
