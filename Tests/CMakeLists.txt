#[[

Unit tests for foleys_gui_magic

The Catch2 library is used to build the unit test runner. Catch2 is fetched at
configure time using FetchContent.

The option FGM_TEST_INSTALL, when on, calls find_package(foleys_gui_magic) and 
builds the unit test runner against this installed package. This can be used to
test the installation process. Defaults to off.

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

include (FetchContent)

FetchContent_Declare (Catch2 
					  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
					  GIT_TAG origin/devel)

FetchContent_MakeAvailable (Catch2)

if(catch2_SOURCE_DIR)
    list (APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
endif()

include (Catch)

juce_add_console_app (FoleysGUIMagicTests VERSION ${FOLEYS_VERSION}
		BUNDLE_ID "com.foleysfinest.foleys_gui_magic.tests")

target_sources (FoleysGUIMagicTests PRIVATE 
					foleys_MagicProcessorTests.cpp 
					foleys_GuiTreeTests.cpp
					foleys_TestProcessors.h)

set_target_properties (
	FoleysGUIMagicTests
	PROPERTIES LABELS "foleys_gui_magic;Tests" 
			   FOLDER foleys_gui_magic 
			   EchoString "Building the foleys_gui_magic unit test runner..."
			   MACOSX_BUNDLE OFF)

target_link_libraries (FoleysGUIMagicTests PRIVATE 
							Catch2::Catch2WithMain 
							foleys::foleys_gui_magic)

target_compile_definitions(FoleysGUIMagicTests
		PUBLIC
		JUCE_SILENCE_XCODE_15_LINKER_WARNING=1)

catch_discover_tests (
    FoleysGUIMagicTests
    TEST_PREFIX foleys_gui_magic.
    EXTRA_ARGS
    	--warn NoAssertions
    	--order rand
    	--verbosity high
    PROPERTIES
    	SKIP_REGULAR_EXPRESSION "SKIPPED:"
    	FAIL_REGULAR_EXPRESSION "FAILED:"
)
