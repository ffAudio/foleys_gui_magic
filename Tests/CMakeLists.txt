#[[

Unit tests for foleys_gui_magic

The Catch2 library is used to build the unit test runner. 
A simple find module is included in the Cmake/ directory -- if no system install 
of Catch2 is found, the sources will be fetched from GitHub at configure time.

The option FGM_TEST_INSTALL, when on, calls find_package(foleys_gui_magic) and 
builds the unit test runner against this installed package. This can be used to
test the installation process. Defaults to off.

For each category of tests, a CTest test will be created, which executes the 
unit test runner and runs only the tests in that category. Each test will be
named `foleys_gui_magic.<category>`.

]]

cmake_minimum_required (VERSION 3.15 FATAL_ERROR)

project (FoleysGUIMagicTests
         VERSION ${FGM_VERSION}
         DESCRIPTION "Unit tests for the foleys_gui_magic JUCE module"
         HOMEPAGE_URL "https://foleysfinest.com/developer/pluginguimagic/"
         LANGUAGES CXX)

enable_testing()

# cmake-format: off
# building command-line tools for iOS isn't supported by Apple's SDKs
if (IOS
	OR ("${CMAKE_SYSTEM_NAME}" STREQUAL iOS)
	OR ("${CMAKE_SYSTEM_NAME}" STREQUAL tvOS)
	OR ("${CMAKE_SYSTEM_NAME}" STREQUAL watchOS))
	
	if(PROJECT_IS_TOP_LEVEL)
		message (FATAL_ERROR "Building command-line tools for iOS isn't supported by Apple's SDKs;
			the foleys_gui_magic unit test executable cannot be crosscompiled for iOS")
	endif()

	return ()
endif ()
# cmake-format: on

option (FGM_TEST_INSTALL 
		"Build the tests against a system install of foleys_gui_magic, instead of the copy in this repo"
		"${PROJECT_IS_TOP_LEVEL}")

if (FGM_TEST_INSTALL OR PROJECT_IS_TOP_LEVEL)
	set (CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

	find_package (foleys_gui_magic "${PROJECT_VERSION}" EXACT REQUIRED CONFIG)
endif ()

if(PROJECT_IS_TOP_LEVEL)
	list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../CMake")
endif()

find_package (Catch2 3 REQUIRED)

juce_add_console_app (FoleysGUIMagicTests VERSION ${FOLEYS_VERSION}
		BUNDLE_ID "com.foleysfinest.foleys_gui_magic.tests")

target_sources (FoleysGUIMagicTests PRIVATE 
					foleys_MagicProcessorTests.cpp 
					foleys_GuiTreeTests.cpp)

set_target_properties (
	FoleysGUIMagicTests
	PROPERTIES LABELS "foleys_gui_magic;Tests" 
			   FOLDER foleys_gui_magic 
			   EchoString "Building the foleys_gui_magic unit test runner..."
			   MACOSX_BUNDLE OFF)

target_link_libraries (FoleysGUIMagicTests PRIVATE 
							Catch2::Catch2WithMain 
							foleys_gui_magic)

get_target_property (tests_sources FoleysGUIMagicTests SOURCES)

source_group (TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${tests_sources})

# execute each category of unit test cases as a separate test
set (test_categories processor gui)

foreach (tag IN LISTS test_categories)
	add_test (NAME "foleys_gui_magic.${tag}" 
			  COMMAND FoleysGUIMagicTests "[${tag}]" 
			  			--warn NoAssertions 
			  			--order rand 
			  			--colour-mode ansi 
			  			--verbosity high)

	set_tests_properties ("foleys_gui_magic.${tag}" PROPERTIES LABELS "foleys_gui_magic;UnitTests")
endforeach ()
