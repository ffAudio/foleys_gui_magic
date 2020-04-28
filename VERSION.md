PluginGuiMagic - Versions history
================================

1.1.4 - 28.04.2020
------------------

- Allow to create dynamic properties by editing the combobox
- Refactor of MagicPlotComponent to redraw only with defined FPS
- Let Tabbed View use the LookAndFeel

1.1.3 - 22.04.2020
------------------

BREAKING CHANGE:
- Properties from parent's style class are no longer inherited by default.
  This can be changed back by checking the "recursive" flag in the style class

Changes:

- Added conditional style classes, that react to dynamic properties
- Added minimum and maximum size for the editor
- Bugfix: Avoid assert when an item is dropped on itself

1.1.2 - 16.04.2020
------------------

- Resizable can be switched on and off, together with default size
- Added class conditions to allow responsive layouts (media tag)
- Bugfix: Selected Tab will stay when changes are made via editor

1.1.1 - 06.04.2020
------------------

- Added tabbed containers
- Bugfix: Fixed not showing XYDraggerComponent

1.1.0 - 04.04.2020
-------------------

- Added note of the dual licensing
- Label can display a dynamic text
- Added font-size and justification to Label
- Toolbox can be detached and set always on top (thanks Benedikt)

1.0.9 - 30.03.2020
------------------

- Removed MagicGuiBuilder template: capture the app instead when registering the factory
- Bugfix: Fixed editor becomming unresponsive when a class was selected via the ComboBox

1.0.8 - 27.03.2020
------------------

- Added option to delete style class
- Bugfix: Fixed wrong entries in plot source menu

1.0.7 - 27.03.2020
------------------

- Added Properties to connect to ToggleButtons
- Added Visibility switch via Properties
- Added trigger option for Buttons
- Display parameters in groups for selection
- Added glow effect to Oscilloscope
- Bugfix: Keep current edits in state even after load XML

1.0.6 - 22.03.2020
------------------

- Property editor displays many default values
- Bugfix: update property editor when deleting a property

1.0.5 - 19. March 2020
----------------------

- Added alpha value for background images
- Added key-width for MidiKeyboardComponent
- Bugfix: avoid infinite loop in Oscilloscope
- Bugfix: don't assert, if AudioProcessorValueTreeState has no ValueTree state set up
- Bugfix: fixed update of layout if a property changes

1.0.4 - 10. March 2020
----------------------

- Added swatches to colour picker
- Added persistency for colour swatches
- Allows typing colour names and copy paste colours
- Added style classes selector
- Added switch for OpenGL

1.0.3 - 06. March 2020
----------------------

- Bugfix: update layout if child order was changed
- Fixed warnings on windows VS 2019

1.0.2 - 05. March 2020
----------------------

- Store current edit in plugin state
- Added ColourSelector for editor
- Added LevelMeters
- Added MidiKeyboardComponent
- Added fill for Plots
- Improved accumulative error in analyser
- Optimised update of colours


1.0.1 - 27. October 2019
------------------------

- Added XY-Dragging component
- Added Analyser plot
- Added Oscilloscope
- Added new LookAndFeel with 3D Skeumorphic effects


1.0.0 - 19. September 2019
--------------------------

- Initial version
- Layout via FlexBox
- Recreate Components from ValueTree
- Connect to AudioParameters
