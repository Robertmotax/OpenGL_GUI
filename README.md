## Screen Shot
<img width="1919" height="984" alt="image" src="https://github.com/user-attachments/assets/42c0f2b0-a40c-45bd-a5b3-b5b96668fad3" />


## Author

**Marcus Sharma Student ID:40240069**

**Robert Chen Student ID:40241709**  


## OpenGL 3D Scene with GUI and Camera

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

Interaction:

- **Sidebar Pagination Controls:**  
  The sidebar possess multiple pages of controls. These keyboard shortcuts enable navigation between pages:  
  - Press **Q** to move to the **previous page** of the sidebar.  
  - Press **E** to move to the **next page** of the sidebar.


Click on objects in scene to edit them using the Sidebar.

The first row of arrows in the sidebar translates the clicked object, the second rotates, the third scales.

Additionally you can manipulate textures by selecting a texture in the sidebar.

Lights are represented as cubes, the rotation manipulates their color and scale manipulates their intensity.

You can drag objects to move them around the scene aswell.

Additional Notes
The camera's pitch is clamped between -89° and +89° to prevent flipping.
