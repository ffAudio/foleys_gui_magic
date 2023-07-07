#[[

This module provides utility functions for setting up pluginval test cases.

This module searches for pluginval in your PATH; this module currently does not download or build 
pluginval if it can't be found, it will simply issue a warning and not create any test cases.

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

mark_as_advanced (PLUGINVAL_PROGRAM PLUGINVAL_STRICTNESS PLUGINVAL_REPEATS PLUGINVAL_SAMPLERATES PLUGINVAL_BLOCKSIZES)

#

#[[

This function registers CTest test cases to run pluginval on the `pluginTarget`. 

Of the formats we build, pluginval supports VST3, VST2, and AU (on Mac). Each one will be tested 
as a separate CTest test case, named `pluginval.<pluginTarget>.<format>`.

This function does nothing if the `LBA_TESTS` option is not enabled.

On Mac, pluginval can only find audio units that are in the system AU directories, so the AU test
will only be added if the `JUCE_COPY_PLUGIN_AFTER_BUILD` option is enabled.

`pluginTarget` should be the name of your plugin's shared code target (the name you passed to
`juce_add_plugin`).

]]
function (lba_add_pluginval_tests pluginTarget)

	if (NOT TARGET "${pluginTarget}")
		message (
			FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} - plugin target ${pluginTarget} does not exist!")
	endif ()

	if (NOT (LBA_TESTS AND PLUGINVAL_PROGRAM))
		return ()
	endif ()

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
			NAME "lobith.pluginval.${pluginTarget}.${formatName}"
			COMMAND "${PLUGINVAL_PROGRAM}"
						--strictness-level "${PLUGINVAL_STRICTNESS}"
						--sample-rates "${sample_rates}"
						--block-sizes "${block_sizes}"
						--repeat "${PLUGINVAL_REPEATS}" --randomise
						--validate "${plugin_artefact}")

		message (VERBOSE "Added pluginval test for plugin target ${format_target}")

	endforeach ()

endfunction ()
