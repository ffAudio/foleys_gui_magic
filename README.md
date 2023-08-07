foleys_gui_magic
===============

DEPRECATION:
the branch `develop` won't be updated and will be deleted soon.    
The development continues in `devel` and will be merged regularly to `main`, so you can always fast-forward the main branch.

main:    
[![CMake](https://github.com/ffAudio/foleys_gui_magic/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/ffAudio/foleys_gui_magic/actions/workflows/build.yml)    

This module allows to create GUIs without any coding. It is created with a DOM model
that provides a hierarchical information, and a CSS cascading stylesheet to define
rules for the appearance of the GUI.

There is a drag and drop editor to add GUI elements, and to connect to
parameters of your AudioProcessor. Also an editor in the style of FireBug
to investigate the individual properties, and how they were obtained/calculated.

Repository map
--------------

The module to add to your project is in `modules/foleys_gui_magic`. Ideally you just use the `juce_add_module()` in your CMakeLists.txt or add it via Projucer.

In the folder `Examples` you find the example projects. The build will run pluginval on the examples (where it applies).    
The CMake option for the examples is `FOLEYS_BUILD_EXAMPLES` and for pluginval `FOLEYS_RUN_PLUGINVAL`.

In `Tests` there are the Catch2 unit tests. Each build will run those unit tests against the latest version.    
The CMake option for the unit tests is `FOLEYS_BUILD_TESTS`.

CMake will be responsible to fetch JUCE, pluginval and Catch2 if switched on.

Support
-------

For the modules of Foleys Finest Audio there is a new forum where you can search for information 
and ask questions: https://forum.foleysfinest.com
All feedback is welcome.

Setup
-----

To use the WYSWYG plugin editor, add this module via Projucer or CMake to your JUCE project.

Instead of inheriting from juce::AudioProcessor inherit foleys::MagicProcessor.
Get rid of those methods:
- bool hasEditor()
- juce::PluginEditor* createEditor()
- void setStateInformation() and void getStateInformation() (optional, a default saving and loading method is provided)

The foleys::MagicProcessor will provide a `foleys::MagicProcessorState magicState` (protected visibility) 
to add visualisations or other objects to expose to the GUI.

The floating editor has an auto save, which needs to know the location of your source files. To activate that
you need to copy this macro into your foleys::MagicProcessor constructor:
```cpp
FOLEYS_SET_SOURCE_PATH(__FILE__);
```
Otherwise autosave will start working once you loaded or saved the state, since then the editor has a location to use.

It is also possible to use the module in an Application. In that case you add a `MagicGuiState` and a `MagicGUIBuilder` yourself.
There is an example available in the examples repository called PlayerExample.


Add Visualisations
------------------

To add visualisations like an Analyser or Oscilloscope to your plugin, add them in the constructor:

```
// Member:
foleys::MagicPlotSource* analyser = nullptr;

// Constructor
analyser = magicState.createAndAddObject<foleys::MagicAnalyser>("input");

// prepareToPlay
analyser->prepareToPlay (sampleRate, samplesPerBlockExpected);

// e.g. in processBlock send the samples to the analyser:
analyser->pushSamples (buffer);
```

In your editor, drag a `Plot` component into the UI and select the name you supplied as `source`, in this
case "input".


Saving and restoring the plugin
-------------------------------

The `foleys::MagicProcessor` takes care of saving and restoring the parameters and properties.
You can add your own values to the ValueTree in the `magicState`.
There is a callback after restoring that you can override, in case you need to do some additional action.

Additionally to saving the values in the plugin state you can have settings across all instances of your plugin.
To use that you need to setup the path to a file. After that the settings are synchronised in the
settings of the magicState:
```
// in constructor
magicState.setApplicationSettingsFile (juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                                       .getChildFile (ProjectInfo::companyName)
                                       .getChildFile (ProjectInfo::projectName + juce::String (".settings")));

// use it like
presetNode = magicState.getSettings().getOrCreateChildWithName ("presets", nullptr);
presetNode.setProperty ("foo", 100, nullptr);
```

Other than the juce::ApplicationSettings this is not a flat list but can deal with hierarchical data in form of ValueTrees.
PluginGuiMagic will deal with interprocess safety and update if the file has changed.

Bake in the xml
---------------

To bake in the GUI save the XML from the PGM panel and add it to the BinaryResources via Projucer.
In the constructor you set the XML file:
```
magicState.setGuiValueTree (BinaryData::magic_xml, BinaryData::magic_xmlSize);
```


Removing the WYSWYG editor
--------------------------

The WYSWYG editor panel (attached outside the PluginEditor) is switched on or off with the config
in Projucer named `FOLEYS_SHOW_GUI_EDITOR_PALLETTE`. An effective way is to add `FOLEYS_SHOW_GUI_EDITOR_PALLETTE=0`
into extra preprocessor defines of the release build, that way you can design in debug mode, and avoid
the WYSWYG editor from being included in the release build for deployment.


Currently available Components
------------------------------

It is completely possible to register your own bespoke Components into the builder. These Components
are already available:

- Slider (attachable to parameters)
- ComboBox ( -"- )
- ToggleButton ( -"- )
- TextButton ( -"- )
- XYDragComponent (attachable to two parameters)
- Plot (displays various 2-d data)
- LevelMeter (displays different RMS / Max levels)
- Label
- MidiKeyboardComponent
- MidiLearn
- ListBox
- WebBrowserComponent

All Components have the option to add margins/paddings and a border also with rounded corners.
The View component serves as container, that has the option to layer all child components on top of each other
or to layout them using flex-box.


Demo projects
-------------

You find the demo projects in the folder `Examples`. We use CMake for building the examples and running the unit tests.

Contributing
------------

Everybody is welcome and encouraged to contribute. This is supposed to be helpful for as 
many people as possible, so please give your ideas as github issues and send pull requests.

We might ask you to change things in your pull requests to keep the style consistent and
to keep the API as concise as possible.

We have a discord server to discuss features, bugs or ideas:  
[Discord](https://discord.gg/gxtjTx6fNg)

If you make changes, make sure that the unit tests in `Tests` still succeed and the examples in the `Examples` folder still compile and work.

Good luck and happy coding, erm designing, erm... both actually :-)


Brighton, UK - started Sept. 2019
Esslingen, Germany - 2020 - 2023
