Program 3
=========

[Assignment Instructions](ASSIGNMENTREADME.md)

For this project, I built a small castle.

For the first person camera controls, I made several improvements.
First was to change how the mouse listening worked.
Instead of clicking and dragging the mouse around, you press q to enter first person mode.
This makes the mouse disapear and move the center of the screen.
When you move the mouse, it tracks the movement and then moves back to center, making it feel like a real first person camera.
To get the mouse back, press q again.
I also implemented head bobbing to make a more realistic walking effect.
This can be toggled on and off with the h key.
Speed can be changed by scrolling and the head bobbing rate will change, but admittedly not in a very realistic way.
The final improvement I made to the first person controls was collision detection.
It is only implemented on the walls because it became too hard to navigate when I put it on every object.
It works by having lines in the xz plane which, if you get too close, it pushes you back.
To toggle collision detection, either enter fly mode with tab or hit the c key.

For lighting, I have 2 types of sources, the sun, which gives objects high ambient lighting, and torches, which have a lower ambient light.
The torches also flicker to make them more realistic looking. The third material is the lights themselves, which only have a high ambient component;

For my program 2 objects, I created a wall, the towers, and the roof of the building. The towers are made of 2 objects, the walls and the roof.
I originally had the building made out of bricks, but it caused too much lag in the system.
The other objects in the world are a ground plane, several planes to make the building and roof, torches, couches, a bookshelf, a throne, a bed, a table, several chairs, and doors.

My animated objects are the doors. When you get within a certain distance of them, they automatically open, and when you move far enough away, they close again.

My partivle effect is a simple fire for each torch, I set a starting x and z velocity which is random and give each an constant upwards acceleration.
