cmake_minimum_required(VERSION 3.13.0)

project("FoleysSynth" VERSION ${FGM_VERSION})

set(${PROJECT_NAME}_sources
    Source/FoleysSynth.cpp
    Source/FoleysSynth.h
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PresetListBox.h)

# add the plugin targets
juce_add_plugin(${PROJECT_NAME}
                VERSION "${version}"
                COMPANY_NAME "Foleys Finest Audio"
                PLUGIN_MANUFACTURER_CODE "FFAU"
                PLUGIN_CODE "PgmS"
                IS_SYNTH yes
                NEEDS_MIDI_INPUT yes
                FORMATS ${FORMATS}
                VST3_CATEGORIES "Instrument" "Synth"
                AAX_CATEGORY "AAX_ePlugInCategory_Instrument"
                AU_MAIN_TYPE "kAudioUnitType_MusicDevice"
                COMPANY_WEBSITE "https://foleysfinest.com"
                COMPANY_EMAIL "info@foleysfinest.com"
                BUNDLE_ID "com.foleysfinest.FoleysSynth"
                PLUGIN_NAME "PGM-FoleysSynth"
                PRODUCT_NAME "PGM-FoleysSynth")

juce_add_binary_data(${PROJECT_NAME}_data
    SOURCES
    Resources/Logo.png
    Resources/magic.xml)

juce_generate_juce_header (${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
    ${${PROJECT_NAME}_sources})

# add required flags
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${project_sources}
    ${PROJECT_NAME}_data
    foleys_gui_magic
    juce::juce_audio_basics
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_cryptography
    juce::juce_gui_extra
    juce::juce_opengl
    juce::juce_recommended_warning_flags 
    juce::juce_recommended_config_flags 
    juce::juce_recommended_lto_flags)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC 
    IS_SYNTH=1
    # switch the following off in the product to hide editor
    FOLEYS_ENABLE_BINARY_DATA=1
    FOLEYS_SHOW_GUI_EDITOR_PALLETTE=1
    FOLEYS_SAVE_EDITED_GUI_IN_PLUGIN_STATE=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_SILENCE_XCODE_15_LINKER_WARNING=1
    JUCE_WEB_BROWSER=0)

foreach(FORMAT ${FORMATS})
    get_target_property(ARTEFACTS_DIR ${PROJECT_NAME}_${FORMAT} LIBRARY_OUTPUT_DIRECTORY)
    add_custom_command(TARGET ${PROJECT_NAME}_${FORMAT} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${ARTEFACTS_DIR} ${COPY_FOLDER})
endforeach()

__pgm_internal_add_pluginval_tests (${PROJECT_NAME})
