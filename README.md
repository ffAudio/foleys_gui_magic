foleys_gui_magic
===============

This module allows to create GUIs without any coding. It is created with a DOM model
that provides a hierarchical information, and a CSS cascading stylesheet to define
rules for the appeareance of the GUI.

There will be a drag and drop editor to add GUI elements, and to connect to
parameters of your AudioProcessorValueTreeState. Also an editor in the style of FireBug
to investigate the individual properties, and how they were obtained/calculated.


Status
------

foleys_gui_magic is a fresh take on ff_layouts and ff_meters. In addition to just layouting 
the Components, foleys_gui_magic sets the colours according to the information defined
in the stylesheet and the position in the DOM.

There is a default tree created from reading the processors `getParameterTree()` method,
so it is easy to get an appealing starting point or even a fully functional generic processor
editor with almost no effort.

The designer can create style sheets, so every default editor is already styled in a
consistent and appealing way. You can even create several style sheets and select from them.

There will be the chance to register measure points inside your code to visualise the
signal with an oscilloscope, the level with a meter or plotting curves including an analyser
spectrum.


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


Brighton, UK - Sept. 2019
