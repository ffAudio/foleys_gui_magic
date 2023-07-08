# simple find module for JUCE
# downloads the sources from GitHub at configure time

include_guard (GLOBAL)

cmake_minimum_required (VERSION 3.15 FATAL_ERROR)

include (FetchContent)
include (FeatureSummary)
include (FindPackageMessage)

set_package_properties ("${CMAKE_FIND_PACKAGE_NAME}" 
						PROPERTIES 
							URL "https://juce.com/"
                        	DESCRIPTION "Cross platform framework for plugin and app development")

FetchContent_Declare (JUCE 
					  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
					  GIT_TAG origin/master)

FetchContent_MakeAvailable (JUCE)

find_package_message ("${CMAKE_FIND_PACKAGE_NAME}" 
					  "JUCE - sources downloaded from GitHub"
                      "JUCE [GitHub]")

set (${CMAKE_FIND_PACKAGE_NAME}_FOUND TRUE)
