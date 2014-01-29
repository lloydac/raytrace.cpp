A fairly simple raytracer I wrote as a final project for my graphics class. Just run `make` to compile on OS X or Linux. Then run the generated `raytrace` executable. It will render the scene and then write it as a PNG file to `raytrace.png` in the current working directory.

Features of my raytracer include: 
- the ability to display spheres, planes, and rectangles
- texture mapping for rectangles
- orthographic and perspective viewing
- point light sources
- full-screen anti-aliasing

If you want to modify the drawing parameters of the scene, just modify the variables at the top of `raytrace.cpp`. If you want to change the scene itself, just modify the `createScene` function in `raytrace.cpp`. If you want to extend the raytracer with more types of objects, just extend the `GeometricObject` class from `scene.h` (which will involve also creating your own extension of the `Intersection` class).