#[[

 Adding this include will add juce to your project

]]

#Adds all the module sources so they appear correctly in the IDE
set_property(GLOBAL PROPERTY USE_FOLDERS YES)
option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Enable Module Source Groups" ON)

#set any of these to "ON" if you want to build one of the juce examples
#or extras (Projucer/AudioPluginHost, etc):
option(JUCE_BUILD_EXTRAS "Build JUCE Extras" OFF)
option(JUCE_BUILD_EXAMPLES "Build JUCE Examples" OFF)

FetchContent_Declare(juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
#       GIT_TAG 7.0.9
        GIT_SHALLOW ON)

FetchContent_MakeAvailable(juce)
