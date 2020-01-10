foleys_gui_magic
===============

This module allows to create GUIs without any coding. It is created with a DOM model
that provides a hierarchical information, and a CSS cascading stylesheet to define
rules for the appeareance of the GUI.

There will be a drag and drop editor to add GUI elements, and to connect to
parameters of your AudioProcessorValueTreeState. Also an editor in the style of FireBug
to investigate the individual properties, and how they were obtained/calculated.


Setup
-----

To use the WYSWYG plugin editor, add this module via Projucer to your JUCE project.
Remove the PluginEditor, that was automatically created by Projucer. Instead add a member
called `foleys::MagicProcessorState` to your processor and create a `foleys::MagicPluginEditor`
in the `createEditor()` method of your processor:

```
// assumes an AudioProcessorValueTreeState named treeState
foleys::MagicProcessorState magicState { *this, treeState };

// return a new editor:
AudioProcessorEditor* EqualizerExampleAudioProcessor::createEditor()
{
    // MAGIC GUI: create the generated editor, load your GUI from magic.xml in the binary resources
    //            if you haven't created one yet, just give it a magicState and remove the last two arguments
    return new foleys::MagicPluginEditor (magicState, BinaryData::magic_xml, BinaryData::magic_xmlSize);
}
```


Add Visualisations
------------------

To add visualisations like an Analyser or Osciloscope to your plugin, add them in the constructor
to the `foleys::MagicPluginState`:

```
// Member:
foleys::MagicPlotSource* analyser = nullptr;

// Constructor
analyser = magicState.addPlotSource ("input", std::make_unique<foleys::MagicAnalyser>());

// e.g. in processBlock send the samples to the analyser:
analyser->pushSamples (buffer);
```

In your editor, drag a `Plot` component into the UI and select the name you supplied as `source`, in this
case "input".


Saving and restoring the plugin
-------------------------------

You can save and restore your plugin state, just as you have been used to. But you can also delegate
this to the `foleys::MagicPluginState`, which will additionally save and restore your PluginEditor's 
last size:

```
void getStateInformation (MemoryBlock& destData) override
{
    // MAGIC GUI: let the magicState conveniently handle save and restore the state.
    //            You don't need to use that, but it also takes care of restoring the last editor size
    magicState.getStateInformation (destData);
}

void setStateInformation (const void* data, int sizeInBytes) override
{
    // MAGIC GUI: let the magicState conveniently handle save and restore the state.
    //            You don't need to use that, but it also takes care of restoring the last editor size
    magicState.setStateInformation (data, sizeInBytes, getActiveEditor());
}
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
