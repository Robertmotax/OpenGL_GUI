## Screen Shot

<img width="1919" height="991" alt="image" src="https://github.com/user-attachments/assets/ccebaee8-8001-4d53-8875-084aef6a1fb0" />



## Author

**Marcus Sharma | Student ID:40240069**

**Robert Chen | Student ID:40241709**  


## OpenGL Object Animation Tool

Built using only OpenGL and the necessary libraries (GLFW, GLEW, GLM, and stb_image.h), this project is a C++ OpenGL application. 
It has the following features:
- An interactive 3D camera that uses arrow keys and WASD
- A modular shader system for real-time rendering.
- Texture support (via stb_image).
- A lighting system using point lights.
- Rendering of custom objects (prism, cubes, complex shapes, etc.)
- Object manipulation tools.
- Object loading and saving features.
- Keyframe tool to add custom animations with hierarchical objects.


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

- **Sidebar Object Editing Controls:**  
Click on objects in scene to edit them using the Sidebar.

  The first row of arrows in the sidebar translates the clicked object, the second rotates, the third scales. You can drag objects to move them around the scene aswell. Lights are represented as cubes, the rotation manipulates their color and scale manipulates their intensity.
  
  Inheritence properties can be set using the set parent and delete parent buttons. To set a parent simply click set parent, then click on the object you wish to make the parent of the selected object.
  
  Additionally you can manipulate textures by selecting a texture in the sidebar.
  
    <img width="139" height="596" alt="image" src="https://github.com/user-attachments/assets/7bc422a9-5e45-46b2-89f5-adccfff53d30" />


- **Sidebar Object Loading Controls:**  
Cubes can be loaded using the second sidebar page. In additon lights can be loaded using the add light button. Both can be texture mapped using a t shaped texture grid like the following.

  ![CubeTexture](https://github.com/user-attachments/assets/420c5e57-1819-42d8-9530-6b162403e48b)

  Previously made '.obj' files can be loaded using the colored squares which represent the objects sequentially from the 'assets/models' folder. The buttons at the bottom allow the scene to be saved to the folder or cleared.
  
    <img width="138" height="597" alt="image" src="https://github.com/user-attachments/assets/1dbb8341-5136-44a1-baf1-b3fa192a2bbd" />

- **Sidebar Object Animation Controls:**  
The time of the animation can be altered using the timelines at the top of the third sidebar page. At each time interval a keyframe can be added for each object allowing its position to be saved at that time. Once two keyframes are added, each point in time will interpolate between the two closest keyframes. With the play button the user can see their animation.

  <img width="139" height="596" alt="image" src="https://github.com/user-attachments/assets/0982222d-0b3a-453a-9e4b-e9a6c00927a5" />


Additional Notes:

- The camera's pitch is clamped between -89° and +89° to prevent flipping.

Issues:

- Lights cannot be animated currently.
