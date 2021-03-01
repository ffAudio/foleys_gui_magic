foleys_gui_magic
===============

This module allows to create GUIs without any coding. It is created with a DOM model
that provides a hierarchical information, and a CSS cascading stylesheet to define
rules for the appearance of the GUI.

There is a drag and drop editor to add GUI elements, and to connect to
parameters of your AudioProcessorValueTreeState. Also an editor in the style of FireBug
to investigate the individual properties, and how they were obtained/calculated.


Support
-------

For the modules of Foleys Finest Audio there is a new forum where you can search for information 
and ask questions: https://forum.foleysfinest.com
All feedback is welcome.

Setup
-----

To use the WYSWYG plugin editor, add this module via Projucer to your JUCE project.

Instead of inheriting from juce::AudioProcessor inherit foleys::MagicProcessor.
Get rid of those methods:
- bool hasEditor()
- juce::PluginEditor* createEditor()
- void setStateInformation() and void getStateInformation() (optional, a default saving and loading method is provided)

The foleys::MagicProcessor will provide a `foleys::MagicProcessorState magicState` (protected visibility) 
to add visualisations or other objects to expose to the GUI.

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

There is a repository with demo projects, that includes this module as git submodule:
[PluginGuiMagic examples](https://github.com/ffAudio/PluginGuiMagic)


Contributing
------------

Everybody is welcome and encouraged to contribute. This is supposed to be helpful for as 
many people as possible, so please give your ideas as github issues and send pull requests.

We might ask you to change things in your pull requests to keep the style consistent and
to keep the API as concise as possible.

We have a repository containing example projects, that are our reference what must not break.
These will be built on our CI for OSX and Windows 10, so we catch hopefully any breakage 
early on. 
Clone that repository using:
```
git clone --recurse-submodules https://github.com/ffAudio/PluginGuiMagic.git
```
To update:
```
git pull origin master
git submodule update
```


Brighton, UK - started Sept. 2019
