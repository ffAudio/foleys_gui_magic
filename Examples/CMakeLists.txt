# Add examples

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
