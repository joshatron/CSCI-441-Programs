Program 2
=========

[Assignment Instructions](ASSIGNMENTREADME.md)

For this project, my main focus was on construction algorithms.
The bricks are held in shape classes.
Each shape is composed of walls (press 1).
Most shapes are composed of walls being stitched together with the exception of 4 sides, where specific work was done to prevent collisions (press 2).
Other shapes have collisions because they tended to look better when the walls were combined (press 3).
Along with straight walls, a function can be applied to the width at different heights, for example of a linear function (press 4).
A function can also be run backward (press 5).
Any function can be created.
For example, the sin function can be translated and scaled down creates a nice vase shape (press 6).
You can also make step functions.
A structure is a construction of one or more shapes which are translated to relative positions in the structure.
For example, combining a wall, four towers, and a dome roofed building will make a simple castle (press 7).
Using 2 oval walls with 500 sides each and 1 linear wall with 500 sides makes a stadium (press 8).

Scaling can be done by brick size and number of bricks.
This scaling can be done independantly on the x, y, and z axis, but may produce strange results if done in a structure with rotated parts (press 9).
The shapes are drawn around an oval so the x and z axis can be scaled independantly.
The way shapes are built can lead to a strange looking corner.
The walls switch whether even or odd rows are longer.
Because of this, an odd number of sides leads to one of the connections not lining up well (press 0).
It was decided not to fix this because you have to look closely for it to be noticeable and would required a more involved construction algorithm.

The lighting was also improved. There are controls to move the light around, change its color, and change its brightness.
A cube is also placed on screen to give an idea of where the light is and what color it is.

Controls

Camera Controls

* z and scoll- zoom

Light Controls

* w- move light in positive z
* s- move light in negative z
* d- move light in positive x
* a- move light in negative x
* e- move light in positive y
* c- move light in negative y
* i and scroll- brightness
* r and scroll- red value
* f and scroll- green value
* v and scroll- blue value

Brick Controls

* t and scroll- brick width
* y and scroll- brick height
* u and scroll- brick width
* b and scroll- uniform brick scale
* g and scroll- scale number of bricks in x dimension
* h and scroll- scale number of bricks in y dimension
* j and scroll- scale number of bricks in y dimension
* n and scroll- scale number of bricks in in all dimensions
* m and scroll- scale number of bricks in x and z dimensions
* x and scroll- spacing

Misc Controls

* q- reset everything
* 0-9- example structures
