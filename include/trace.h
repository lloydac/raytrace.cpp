#ifndef TRACE_H
#define TRACE_H

#include <primitives.h>
#include <intersection.h>
#include <scene.h>

// draws a scene and loads the resulting pixels into buffer
void drawScene(Scene* s, unsigned char* buffer, int width, int height, int maxDepth, bool orthographic, int antialiasFactor);

// traces a ray and loads the resulting color into c.
// assumes that the direction vector of r is normalized.
void trace(Scene* s, Ray* r, int maxDepth, Color &c);

// finds the object closest to the origin of the ray which the ray intersects
Intersection* findFirstIntersection(Scene* s, Ray* r);

void computeShadow(Scene* scene, Ray* shadow, float lightT, int maxDepth, Color &result);

#endif