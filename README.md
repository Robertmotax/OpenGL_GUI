#OpenGL 3D Scene with GUI and Camera

Built using only OpenGL and the necessary libraries (GLFW, GLEW, GLM, and stb_image.h), this project is a C++ OpenGL application. 
It has the following features:
- An interactive 3D camera that uses arrow keys and WASD
A modular shader system for real-time rendering; basic GUI integration for vertex movement directly to the user
Texture support (via stb_image)
The lighting system, which includes point, directional, and ambient lights
Rendering of custom objects (floor, cubes, etc.)


Camera Controls
The camera allows free movement and rotation within the 3D scene using the keyboard:

Movement:

Mouse displacement: allows to move left to right and "forward-backward" by moving mouse up and down.


W – Move forward
S – Move backward
A – Move left (strafe)
D – Move right (strafe)

Space – Move up (positive y-axis)
Left Shift – Move down (negative y - axis)
ALT - increase cameraSpeed

Rotation:

Right Arrow – Rotate camera to the right (increase yaw)
Left Arrow – Rotate camera to the left (decrease yaw)
Up Arrow – Look up (increase pitch)
Down Arrow – Look down (decrease pitch)

Exit:
Escape – Close the application window

Additional Notes
The camera's pitch is clamped between -89° and +89° to prevent flipping.
