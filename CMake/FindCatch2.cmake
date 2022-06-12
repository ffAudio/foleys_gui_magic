# simple find module for Catch2 unit testing library
# downloads the sources from GitHub at configure time

include_guard (GLOBAL)

cmake_minimum_required (VERSION 3.15 FATAL_ERROR)

include (FetchContent)
include (FeatureSummary)
include (FindPackageMessage)

set_package_properties ("${CMAKE_FIND_PACKAGE_NAME}" 
						PROPERTIES 
							URL "https://github.com/catchorg/Catch2"
                        	DESCRIPTION "C++ unit testing framework")

FetchContent_Declare (Catch2 
					  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
					  GIT_TAG origin/devel)

FetchContent_MakeAvailable (Catch2)

find_package_message ("${CMAKE_FIND_PACKAGE_NAME}" 
					  "Catch2 - sources downloaded from GitHub"
                      "Catch2 [GitHub]")

set (${CMAKE_FIND_PACKAGE_NAME}_FOUND TRUE)
