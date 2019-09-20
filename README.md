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

This is just starting now, if you want to get involved, play around. But it is not useful yet...

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
