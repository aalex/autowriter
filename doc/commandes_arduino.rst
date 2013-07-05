Controls for the Arduino plotter
================================

Custom Commands
---------------
ZQ: status
~~~~~~~~~~
Doesn't do anything.

Standard HPGL Commands
----------------------

IN: INitialize plotter
~~~~~~~~~~~~~~~~~~~~~~
Reinits the settings.

Numbers of arguments: 0

FS: Force Select (Select Pen-Pressure)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Numbers of arguments: 1

PA: Plot Absolute
~~~~~~~~~~~~~~~~~
Numbers of arguments: 2

Arguments: X,Y

* X ; X coordinate of the cursor movement destination
* Y ; Y coordinate of the cursor movement destination
* X and Y are absolute values in user units or graphics units.
* Moves the cursor to the specified coordinates.
* Plots a straight line only when the pen is down.


PD: Pen Down
~~~~~~~~~~~~
Numbers of arguments: 2

Arguments: x,y

* X ; X coordinate of the cursor movement destination
* Y ; Y coordinate of the cursor movement destination
* X and Y are either relative or absolute, depending on whether a PA or a PR was the last plot command
* executed. The absolute coordinates are set as default.
* Moves the cursor to the specified coordinates after lowering the pen. (This plots a straight line.)
* Using this instruction without a parameter lowers the pen without changing the cursor position. (One dot is plotted.)


PR: Plot Relative
~~~~~~~~~~~~~~~~~
Numbers of arguments: 2

Arguments: x,y

PU: Pen Up
~~~~~~~~~~
Numbers of arguments: 2

Arguments: x,y

Should be either relative or absolute.

EP: Select Eraser-Pressure
~~~~~~~~~~~~~~~~~~~~~~~~~~
Numbers of arguments: 1

ED: Eraser Down
~~~~~~~~~~~~~~~
Numbers of arguments: 2

Arguments: x,y

EU: Eraser Up
~~~~~~~~~~~~~~
Numbers of arguments: 2

Arguments: x,y

MO: MOtor control
~~~~~~~~~~~~~~~~~
Numbers of arguments: 1

XP: pause-time for pen/eraser up/down
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Numbers of arguments: 1

XS: set the scaling-factor
~~~~~~~~~~~~~~~~~~~~~~~~~~
Numbers of arguments: more than one (probably 2: x,y)

it is param / 10000, so 10000 = 1:1

XO: set the offset
~~~~~~~~~~~~~~~~~~
Numbers of arguments: more than one (probably 2: x,y)

offest point is scaled with the drawing scale

XR: Acceleration Rate
~~~~~~~~~~~~~~~~~~~~~
Numbers of arguments: 1

set the accelleration/decelleration rate


VS: Velocity select
~~~~~~~~~~~~~~~~~~~
Numbers of arguments: 1

XX: test
~~~~~~~~
Queries the chalk position and the servo high.

