cmake_minimum_required(VERSION 3.13.0)

project(PlayerExample VERSION ${FGM_VERSION})

juce_add_gui_app(${PROJECT_NAME}
        VERSION ${PROJECT_VERSION}
        COMPANY_NAME "Foleys Finest Audio"
        PLUGIN_MANUFACTURER_CODE "FFAU"
        COMPANY_WEBSITE "https://foleysfinest.com"
        COMPANY_EMAIL "info@foleysfinest.com"
        BUNDLE_ID "com.foleysfinest.FoleysSynth"
        PRODUCT_NAME "PGM-PlayerExample"
        NEEDS_CURL FALSE
        NEEDS_WEB_BROWSER FALSE)

juce_add_binary_data(${PROJECT_NAME}_data
        SOURCES
        Resources/magic.xml)

juce_generate_juce_header(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
        Source/Main.cpp
        Source/Waveform.cpp
        Source/Waveform.h
        Source/MainComponent.cpp
        Source/MainComponent.h)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${PROJECT_NAME}_data
        foleys_gui_magic
        juce::juce_audio_basics
        juce::juce_audio_devices
        # juce::juce_audio_plugin_client
        # juce::juce_audio_processors
        juce::juce_audio_utils
        # juce::juce_dsp
        juce::juce_cryptography
        juce::juce_gui_extra
        # juce::juce_opengl
        juce::juce_recommended_warning_flags
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags)

target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        # switch the following off in the product to hide editor
        FOLEYS_ENABLE_BINARY_DATA=1
        FOLEYS_SHOW_GUI_EDITOR_PALLETTE=1
        FOLEYS_SAVE_EDITED_GUI_IN_PLUGIN_STATE=0
        JUCE_WEB_BROWSER=0)
