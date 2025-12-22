# Dardaneli

**Dardaneli** is a 3D modeling and rendering engine inspired by Blender. Made using C++14, OpenGL and ImGUI.

---

## Features

- **Object Picking:** Select objects in the scene using raycasting.  
- **Mesh Editing:** Extrude, delete, merge, linear subdivision, fill, loop cut and more.  
- **UV Editing:** Unwrap meshes using least squares conformal mapping (LSCM) and edit UVs directly.  
- **Shader Editor:** Create custom materials with a node-based material editor.  
- **Rendering:** Physically Based Rendering (PBR) for realistic material and lighting effects.





---
### Demo 

<video width="640" height="360" controls>
  <source src="assets/SeptemberDemo.mp4" type="video/mp4">
  Your browser does not support the video tag.
</video>






---


### Build
```bash
git clone https://github.com/acorovic1/Dardaneli.git
cd Dardaneli
mkdir build 
cd build
cmake ..
cmake --build .
