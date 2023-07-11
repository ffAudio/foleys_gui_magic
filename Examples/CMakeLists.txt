# Add examples

message (STATUS "Build examples")
message (STATUS "Run pluginval on examples: ${FOLEYS_RUN_PLUGINVAL}")

if(FOLEYS_RUN_PLUGINVAL)
    include (PGMPluginval)
endif()

# for our internal use only
# this function only adds pluginval tests if FOLEYS_RUN_PLUGINVAL is on, and uses our test naming convention
function(__pgm_internal_add_pluginval_tests pluginTarget)
    if(FOLEYS_RUN_PLUGINVAL)
        pgm_add_pluginval_tests ("${pluginTarget}" 
            TEST_BASE_NAME "foleys_gui_magic.examples.${pluginTarget}.pluginval")
    endif()
endfunction()


# check which formats we want to build
set(FORMATS "VST3" "Standalone")
if(APPLE OR WIN32) # no AAX on Linux
#    raises error: Use juce_set_aax_sdk_path to set up the AAX sdk before adding AAX targets
#    list (APPEND FORMATS AAX)
endif()

if (VST2_PATH)
    list(APPEND FORMATS "VST2")
    juce_set_vst2_sdk_path(${VST2_PATH})
endif()

if (APPLE)
    list(APPEND FORMATS "AU")
endif()

# setup the copying to the output folder
if (APPLE)
    set(COPY_FOLDER ${CMAKE_SOURCE_DIR}/Builds/MacOSX)
elseif(WIN32)
    set(COPY_FOLDER ${CMAKE_SOURCE_DIR}/Builds/VisualStudio2019)
endif()

add_subdirectory(APVTS_Tutorial)
add_subdirectory(EqualizerExample)
add_subdirectory(ExtendingExample)
add_subdirectory(FoleysSynth)
add_subdirectory(SignalGenerator)
# add_subdirectory(PlayerExample)
