#ifndef GEOMETRY_INTERSECTION_H
#define GEOMETRY_INTERSECTION_H

#include <primitives.h>

class GeometricObject;

/**
 * Class which represents an intersection of a ray and an object.
 *
 * Having each object compute its own normals, colors, and rays at the point of
 * intersection allows for a cleaner separation between the ray tracer logic
 * and the representation of objects, meaning that adding more types of objects
 * should not require changes to the core ray tracer code.
 *
 * I chose to have the various properties returned by virtual methods rather than
 * be stored in fields for potential efficiency gains. For every ray cast, the ray
 * tracer must compute intersections with every object in the scene. However, it
 * will ultimately only end up using one of those intersections. Using virtual methods
 * allows the computation of these properties to be delayed until it is determined
 * which intersection will actually be used.
 */
class Intersection
{
    public:
        // the point of intersection
        Point point;
        
        // the value of the ray's parameter at the point
        // of intersection. should always be nonnegative.
        float t;
        
        // the object that the ray was intersected with
        GeometricObject* object;
        
        // the normal vector should be normalized.
        virtual void getNormal(Vector &n) = 0;
        virtual void getMaterial(Material &m) = 0;
        
        // virtual destructor to stop compiler warnings
        virtual ~Intersection(){}
};

#endif