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




https://github.com/user-attachments/assets/1007bf3e-0427-4902-8c7c-e57eab8eb6ba







---


### Build
```bash
git clone https://github.com/acorovic1/Dardaneli.git
cd Dardaneli
mkdir build 
cd build
cmake ..
cmake --build .
