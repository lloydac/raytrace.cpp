// This file defines structs and classes that can be used to construct
// a complete scene that can then be passed to the ray tracer.
#ifndef GEOMETRY_SCENE_H
#define GEOMETRY_SCENE_H

#include <primitives.h>
#include <intersection.h>

#include <vector>
#include <cstddef>

using std::vector;

struct PointLight;
struct DirectionalLight;

class GeometricObject 
{
    public:
        
        /**
         * Intersects a ray with this object. Returns NULL iff the ray and object don't 
         * intersect. Otherwise returns an object representing the point of intersection.
         * The caller is responsible for deleting the returned Intersection. Note that the
         * returned Intersection may refer to this GeometricObject, so it may not be safe
         * to delete or modify the GeometricObject if you are still using the Intersection.
         *
         * Intersections at or approximately at the ray's origin should not be considered.
         */
        virtual Intersection* intersect(Ray* r) = 0;
};

struct Scene {
    Color backgroundColor;
    Color ambientLight;
    
    // The viewpoint of the scene is always at the origin facing
    // in the negative-z direction, so the viewplane is always
    // perpendicular to the z-axis and should have a negative z
    // value. These parameters specify the extents of the view plane.
    float viewPlaneTop;
    float viewPlaneBottom;
    float viewPlaneLeft;
    float viewPlaneRight;
    float viewPlaneZ;
    
    vector<GeometricObject*> objects;
    vector<PointLight*> pointLights;
    vector<DirectionalLight*> directionalLights;
};

/*************************************************
 ****************** OBJECT TYPES *****************
 *************************************************/

class Sphere : public GeometricObject 
{
    friend class SphereIntersection;
    
    private:
        Point center;
        float radius;
        Material material;
    
    public:
        
        Sphere(Material m, Point center, float radius);
        virtual Intersection* intersect(Ray* r);
};

class Plane : public GeometricObject
{
    friend class PlaneIntersection;
    
    private:
        Point point;
        Vector normal;
        Material material;
    
    public:
        Plane(Material m, Point point, Vector normal);
        virtual Intersection* intersect(Ray* r);
};

class Rectangle : public GeometricObject
{
    friend class RectangleIntersection;
    friend class TexturedRectangleIntersection;
    
    private:
        float xMax, xMin;
        float yMax, yMin;
        float zMax, zMin;
        Vector normal;
        Material material;
    
    public:
        Rectangle(Material m, 
            float xMax, float xMin, float yMax, float yMin, float zMax, float zMin, Vector normal);
        virtual Intersection* intersect(Ray* r);
};

#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2

class TexturedRectangle : public Rectangle
{
    friend class TexturedRectangleIntersection;
    
    private:
        Texture texture;
        int sAxis, tAxis;
    
    public:
        TexturedRectangle(Material m, Texture t,  int sAxis, int tAxis,
            float xMax, float xMin, float yMax, float yMin, float zMax, float zMin, Vector normal);
        virtual Intersection* intersect(Ray* r);
};

// class Cylinder : public GeometricObject
// {
//     friend class CylinderIntersection;
    
//     private:
//         float radius;
//         Point topCenter, bottomCenter;
// };

/*************************************************
 ***************** LIGHT SOURCES *****************
 *************************************************/

struct PointLight {
    Color color;
    Point location;
};

struct DirectionalLight {
    Color color;
    // assumed to be normalized
    Vector direction;
};

#endif