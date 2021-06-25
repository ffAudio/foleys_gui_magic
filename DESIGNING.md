Designing in foleys_gui_magic
=============================

General Setup
-------------

PluginGuiMagic is a layout engine and editor to create GUIs in JUCE without writing code.
The whole GUI is defined as a DOM tree of Components and CSS instructions of visual properties.

Each Component is wrapped into a GuiItem that adds decorations to or around the Components.
The main decorations is a margin, padding and a border, as well as a caption.
The background can be an image, a colour or a colour gradient.

The different panels are hierarchically organised. Each View contains other View containers or Components.
Every View has a property `display` which selects, how the children are layouted. By default this is set to `FlexBox`.
If you set it to `Content` you can drag each child individually to any place. The number is reflected in the 
property view in the side panel under `Node`. By adding a % as suffix the positions and dimensions will become relative
to the parent View instead of in absolute pixels. the third option is Tabbed, which will create a tab bar for the child Views or Components.

CSS Classes
-----------

You can set all properties to the nodes. But the properties are looked up traversing the DOM.
Every Component or View can be assigned a list of CSS classes, so the properties are looked up in those classes too.
The classes have a switch, if the properties shall be used by the children as well or just by the class referencing the CSS class.

Responsive design
-----------------

The layout is managed via FlexBox, which offers a lot of options how to react to resizing.

Additionally the CSS classes have switches to be turned on or off. You can connect that for instance to a checkbox to turn
the visibility on or off. Another method to change the layout rules is to set a min size or max size to a CSS class, which also
switches the class off if the plugin size is smaller than the min size or greater than the max size.

Saving and loading
------------------

To save the GUI use the File menu in the toolbox. You can also use `clear` in the File menu to start from scratch or
use `default` to create a default GUI from the parameters and visualisers found in the processor.

To get the GUI into the product make sure the BinaryData is recreated (which happens automatically in cmake but
using the Projucer you need to save the Project there and recompile.

