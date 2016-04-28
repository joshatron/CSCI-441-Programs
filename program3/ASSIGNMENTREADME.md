# Virtual World

*You must work individually on this assignment. To receive credit, 
push all required materials to your own private fork of this 
repository by the due
date (see the [syllabus](https://bitbucket.org/msucsc441spring2016/syllabus)
for an up-to-date schedule of assignments and due dates). Ensure the
repository is shared with me and email the link to the repository to me.*

## Summary

Virtual worlds are an important part of graphics applications. Objects 
populate a world or scene and the user is able to navigate that world in 
some way. In this assignment, you will be creating a world that the user 
can move around in using first-person controls. You must scatter around the 
objects you generated in program 2, along with some other simpler pieces of 
geometry to create an interesting world to explore. Vary the customizable 
options you had in program 2 to keep the world more interesting. Assign 
different material properties to the different objects in your world. You may 
choose the lighting conditions of your world. Lighting plays an important role 
in how your world looks and feels. Is it mid day? Dusk? Dark with a flashlight 
attached to your virtual character? You must also animate at least one object
in your world to add an extra detail to your world. For example, an object 
could be floating down a stream or a character to could be walking back and 
forth, it's up to you.

## Required Materials

Your program3 directory must include:

* All source code for the completed program
* A screenshot of your world from a viewpoint of your choice
* A README.md file with any necessary instructions for using your program, 
along with a description of what is in your world and what lighting condition
you chose for your world

## Detailed Requirements

*Your program must*

*Be an original program written by you.* You may use code from labs as a starting point. You may talk with other 
students about the program, but looking at their code is not allowed.

*Implement a first person camera control.* The user must be able to move left, 
right, forward and backward using ‘a’, ‘d’, ‘w’ and ‘s’ respectively. The user 
must also be able to look around with the mouse which will change the direction
the movement keys translate the camera. The user must be able to explore your 
world. Collision detection with your objects is not required.

*Light and shade your world using the Phong reflection model.* Your world must 
have objects that are assigned at least 3 different materials. A material 
consists of an ambient, diffuse and specular color. Create lights in your 
world to approximate the lighting condition of your choice. Feel free to 
apply textures to give more detail to your world.

*Construct a virtual world.* Your world must include at least a ground plane 
and 10 different configurations of your program 2 object. It must also include 
at least 10 different basic objects. A basic object could be as simple as a 
couple scaled cubes or could be models that you load in. The goal is to create 
a visually interesting world that the user wants to explore. Hide easter eggs 
if you like and let the user find them by pure exploration, or lead the user 
places with how you’ve laid out your objects. It’s up to you.

*Animate at least one object in your world.* At least one object must be animated 
in your world. The object could be moving on its own to fit the theme of your world
or it could be activated by the user performing some action.

*A README.md file.*  Write up what your program does, and how to use it, along 
with any extra information you want me to know about your program.

*Demonstrate creativity and self expression.* Put effort into your program, 
beyond the minimum requirements. Experiment, put extra features in, organize 
your code exquisitely, it’s really up to you. Explain what you did in your 
README.md file.

## Point breakdown

| Points | Description |
|--------|-------------|
| 20 | Correct first person controls |
| 20 | An animated object |
| 20 | 3 different materials assigned to objects |
| 10 | Lighting that matches your choice of conditions |
| 10 | 20 different objects (10 objcets from program 2, 10 basic objects) |
| 20 | General (code style, execution, creativity, self expression and README.md) |
| *100* | *Total points* |

## Extra Credit

up to 20 points - Add a particle system for the user to find. What the particle
system does is up to you, but could be a sparks simulation, a fountain, a tornado
or whatever else you come up with. The difficulty of what you have it do will
determine how much extra credit you receive. 

| Extra credit | Description |
|--------------|-------------|
|     2        | Use of a particle system |
|     3        | Themed use of a particle system |
|     3        | Rendering the particles as something other than GL_POINTS |
|     6        | Advanced use of a particle system (up to 2 times) |

Advanced uses of particle systems include using complicated forces to create vortices
or other interesting effects. Advanced rendering of the particles can also recieve
points for an advanced use if it achieves a specific visual effect, such as rendering
fire or smoke. These can take a significant amount of time to achieve, so only
attempt to implement them after meeting the other requirements.


