# SoftwareRenderer
Software renderer developed from scratch in C++ 11 including a handmade obj loader.

<p align="center">
  <img src="http://jonathanmcontreras.com/images/portfolio/techniques.gif" height="768" width="1024" alt="Shotgun"/>
</p>

## Features
The software renderer is capable to render OBJ files with support for diffuse, normal and specular textures. It additionally implements a shader system that allows to change shaders in runtime. There are six main shaders to show different techniques that can be changed to at any moment by using the function keys(F1-F6): Phong shading, Gouraud Shading, Normal Face illumination, Clamp illumination, Tangent Space Normals as colour and a representation of the zBuffer.

<p align="center">
  <img src="http://jonathanmcontreras.com/images/portfolio/renderer/zbuffer_diablo.png" height="310" width="432" alt="camera"/>
  <img src="http://jonathanmcontreras.com/images/portfolio/renderer/gouraud_diablo.png" height="310" width="432" alt="camera"/>
  <img src="http://jonathanmcontreras.com/images/portfolio/renderer/face_diablo.png" height="310" width="432" alt="camera"/>
  <img src="http://jonathanmcontreras.com/images/portfolio/renderer/clamp_diablo.png" height="310" width="432" alt="camera"/>
  <img src="http://jonathanmcontreras.com/images/portfolio/renderer/tangent_diablo.png" height="310" width="432"" alt="camera"/>
  <img src="http://jonathanmcontreras.com/images/portfolio/renderer/phong_diablo.png" height="310" width="432" alt="camera"/>
</p>

The renderer supports the possibility to scale, transform and rotate the objects in screen. It also allows to move the camera or the light position.

<p align="center">
  <img src="http://jonathanmcontreras.com/images/portfolio/renderer/face_camera.png" height="310" width="432" alt="camera"/>
  <img src="http://jonathanmcontreras.com/images/portfolio/renderer/face_camera2.png" height="310" width="432" alt="camera"/>
</p>

## Posible improvements
Since the rendering of complex 3D object in software is an heavy task, the vector operations could be improved by implementing SIMD for the dot product and vector normalization.

## Documentation
The best two main sources to learn about rasterization: https://github.com/ssloy/tinyrenderer and www.scratchapixel.com. Both of them offer great introduction to 3D rendering.
