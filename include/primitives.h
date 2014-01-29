// This file defines geometric and graphics primitives.
#ifndef GEOMETRY_PRIMITIVES_H
#define GEOMETRY_PRIMITIVES_H

// number which is approximately zero and can be used to
// compensate for loss of precision with floating point arithmetic
#define EPSILON 0.01

/*************************************************
 *************** GEOMETRIC OBJECTS ***************
 *************************************************/

struct Point {
    float x, y, z;
    
    Point(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    
    Point()
    {
        x = y = z = 0;
    }
};

struct Vector {
    float x, y, z;
    
    Vector(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    
    Vector()
    {
        x = y = z = 0;
    }
    
    // compute the norm/length of the vector
    float normSq();
    float norm();
    
    // returns the normalized version of this vector
    Vector normalize();
};

struct Ray {
    Point origin;
    Vector direction;
    
    Ray(Point o, Vector d)
    {
        origin = o;
        direction = d;
    }
    
    Ray()
    {
        origin = Point();
        direction = Vector();
    }
    
    Point operator()(float t);
};

// point-vector addition
Point operator+(Point lhs, Vector rhs);
Point operator+(Vector lhs, Point rhs);

// point subtraction (creates a vector)
Vector operator-(Point lhs, Point rhs);

// vector addition/subtraction
Vector operator+(Vector lhs, Vector rhs);
Vector operator-(Vector lhs, Vector rhs);

// multiplying a vector by a scalar
Vector operator*(float a, Vector v);
Vector operator*(Vector v, float a);

// vector products
float dot(Vector lhs, Vector rhs);
Vector cross(Vector lhs, Vector rhs);

/*************************************************
 ********************* COLOR *********************
 *************************************************/
 
struct Color {
    float r, g, b;
    
    Color(float r, float g, float b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    
    Color()
    {
        r = g = b = 0.0;
    }
    
    Color operator*=(float a);
    Color operator*=(Color c);
    Color operator+=(Color c);
    
    void clampThis();
};

Color operator*(float a, Color c);
Color operator*(Color c, float a);
Color operator*(Color lhs, Color rhs);
Color operator+(Color lhs, Color rhs);

/*************************************************
 ************** MATERIAL PROPERTIES **************
 *************************************************/

struct Material {
    Color ambient;
    Color diffuse;
    Color specular;
    Color refracted;
    Color emission;
    float shininess;
};

struct Texture
{
    Color* pixels;
    int width;
    int height;
    
    Color operator()(float s, float t);
};

Texture loadTexture(const char* filename);

#endif