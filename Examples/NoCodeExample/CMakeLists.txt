cmake_minimum_required(VERSION 3.13.0)

project("NoCodeExample" VERSION ${FGM_VERSION})

# add the plugin targets
juce_add_plugin(${PROJECT_NAME}
        VERSION "${version}"
        COMPANY_NAME "Foleys Finest Audio"
        PLUGIN_MANUFACTURER_CODE "FFAU"
        PLUGIN_CODE "NoCd"
        IS_SYNTH yes
        FORMATS ${FORMATS}
        VST3_CATEGORIES "Fx" "Analyser" "EQ"
        AAX_CATEGORY "AAX_ePlugInCategory_SWGenerators"
        AU_MAIN_TYPE "kAudioUnitType_Generator"
        COMPANY_WEBSITE "https://foleysfinest.com"
        COMPANY_EMAIL "info@foleysfinest.com"
        BUNDLE_ID "com.foleysfinest.NoCodeExample"
        PLUGIN_NAME "NoCodeExample"
        PRODUCT_NAME "NoCodeExample")

juce_add_binary_data(${PROJECT_NAME}_data
        SOURCES
        Resources/magic.xml)

juce_generate_juce_header(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
        PRIVATE
        NoCodeExample.cpp)

# add required flags
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${PROJECT_NAME}_data
        foleys_dsp_magic
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
        # switch the following off in the product to hide editor
        FOLEYS_SHOW_GUI_EDITOR_PALLETTE=0
        FOLEYS_SAVE_EDITED_GUI_IN_PLUGIN_STATE=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=0)

foreach (FORMAT ${FORMATS})
    get_target_property(ARTEFACTS_DIR ${PROJECT_NAME}_${FORMAT} LIBRARY_OUTPUT_DIRECTORY)
    add_custom_command(TARGET ${PROJECT_NAME}_${FORMAT} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${ARTEFACTS_DIR} ${COPY_FOLDER})
endforeach ()

__pgm_internal_add_pluginval_tests(${PROJECT_NAME})
