#include <scene.h>
#include <cmath>

class SphereIntersection : public Intersection 
{
    public:
        
        virtual void getNormal(Vector &n) 
        {
            Sphere* sphere = static_cast<Sphere*>(object);
            n = (point - sphere->center);
            
            // divide by radius to normalize
            float r = sphere->radius;
            n.x /= r;
            n.y /= r;
            n.z /= r;
        }
        
        virtual void getMaterial(Material &m) 
        {
            Sphere* sphere = static_cast<Sphere*>(object);
            m = sphere->material;
        }
};

Sphere::Sphere(Material m, Point center, float radius)
{
    this->material = m;
    this->center = center;
    this->radius = radius;
}

Intersection* Sphere::intersect(Ray* ray)
{
    float t;
    
    Ray R = *ray;
    Vector D = R.direction;
    Vector P = R.origin - center;
    float r = radius;
    
    float normD = D.normSq();
    float normP = P.normSq();
    float dotDP = dot(D, P);
    
    // under the radical in quadratic equation
    float rad = dotDP*dotDP - normD * (normP - r*r);
    
    if (rad < 0) 
    {
        // no real roots means no intersections
        return NULL;
    }
    
    float sqrtRad = sqrt(rad);
    float t1 = (-dotDP - sqrtRad) / normD;
    float t2 = (-dotDP + sqrtRad) / normD;
    
    if (t1 < EPSILON && t2 < EPSILON)
    {
        // no intersections; ray is behind sphere
        return NULL;
    }
    else if (t1 < EPSILON)
    {
        // only one intersection, must be t2
        t = t2;
    } 
    else 
    {
        // two points of intersection, but t1 is closer
        t = t1;
    }
    
    // we now know we have an intersection
    
    SphereIntersection* i = new SphereIntersection();
    i->t = t;
    i->point = R(t);
    i->object = this;
    
    return i;
}

class PlaneIntersection : public Intersection
{
    public:
        
        virtual void getNormal(Vector &n)
        {
            Plane* plane = static_cast<Plane*>(object);
            n = plane->normal;
        }
        
        virtual void getMaterial(Material &m)
        {
            Plane* plane = static_cast<Plane*>(object);
            m = plane->material;
        }
};

Plane::Plane(Material m, Point point, Vector normal)
{
    this->material = m;
    this->point = point;
    this->normal = normal;
}

Intersection* Plane::intersect(Ray* ray)
{
    Ray R = *ray;
    Vector D = R.direction;
    Vector N = normal;
    Point L = R.origin;
    Point P = point;
    
    float dotDN = dot(D, N);
    
    if (dotDN == 0)
        // line is parallel to plane
        return NULL;
    
    float t = dot(P - L, N) / dotDN;
    
    if (t < EPSILON)
        return NULL;
    
    PlaneIntersection* i = new PlaneIntersection;
    i->t = t;
    i->point = R(t);
    i->object = this;
    
    return i;
}

class RectangleIntersection : public Intersection
{
    public:
        
        virtual void getNormal(Vector &n)
        {
            Rectangle* rect = static_cast<Rectangle*>(object);
            n = rect->normal;
        }
        
        virtual void getMaterial(Material &m)
        {
            Rectangle* rect = static_cast<Rectangle*>(object);
            m = rect->material;
        }
};

Rectangle::Rectangle(Material m, 
    float xMax, float xMin, float yMax, float yMin, float zMax, float zMin, Vector normal)
{
    this->material = m;
    this->normal = normal;
    this->xMax = xMax;
    this->xMin = xMin;
    this->yMax = yMax;
    this->yMin = yMin;
    this->zMax = zMax;
    this->zMin = zMin;
}

Intersection* Rectangle::intersect(Ray* ray)
{
    // first find the intersection with the rectangle's plane
    
    Ray R = *ray;
    Vector D = R.direction;
    Vector N = normal;
    Point L = R.origin;
    Point P = Point(xMax, yMax, zMax);
    
    float dotDN = dot(D, N);
    
    if (dotDN == 0)
        // line is parallel to plane
        return NULL;
    
    float t = dot(P - L, N) / dotDN;
    
    if (t < EPSILON)
        return NULL;
    
    // now check to see if the point of intersection is in the rectangle
    Point point = R(t);
    
    bool checkX = xMin - EPSILON <= point.x && point.x <= xMax + EPSILON;
    bool checkY = yMin - EPSILON <= point.y && point.y <= yMax + EPSILON;
    bool checkZ = zMin - EPSILON <= point.z && point.z <= zMax + EPSILON;
    
    if (!(checkX && checkY && checkZ))
        return NULL;
    
    RectangleIntersection* i = new RectangleIntersection;
    i->t = t;
    i->point = point;
    i->object = this;
    
    return i;
}

class TexturedRectangleIntersection : public Intersection
{
    public:
        
        virtual void getNormal(Vector &n)
        {
            TexturedRectangle* rect = static_cast<TexturedRectangle*>(object);
            n = rect->normal;
        }
        
        virtual void getMaterial(Material &m)
        {
            TexturedRectangle* rect = static_cast<TexturedRectangle*>(object);
            
            float s = computeParam(rect->sAxis, rect);
            float t = computeParam(rect->tAxis, rect);
            Color texel = rect->texture(s, t);
            
            m = rect->material;
            m.ambient *= texel;
            m.diffuse *= texel;
            m.emission *= texel;
        }
    
    private:
        
        float computeParam(int axis, TexturedRectangle* rect)
        {
            switch (axis)
            {
                case XAXIS:
                    return (point.x - rect->xMin) / (rect->xMax - rect->xMin);
                case YAXIS:
                    return (point.y - rect->yMin) / (rect->yMax - rect->yMin);
                case ZAXIS:
                    return (point.z - rect->zMin) / (rect->zMax - rect->zMin);
            }
            
            // to make compiler happy
            return 0;
        }
};

TexturedRectangle::TexturedRectangle(Material m, Texture t, int sAxis, int tAxis,
    float xMax, float xMin, float yMax, float yMin, float zMax, float zMin, Vector normal)
    : Rectangle(m, xMax, xMin, yMax, yMin, zMax, zMin, normal)
{
    this->texture = t;
    this->sAxis = sAxis;
    this->tAxis = tAxis;
}

Intersection* TexturedRectangle::intersect(Ray* ray)
{
    Intersection* i = Rectangle::intersect(ray);
    if (!i) return NULL;
    
    Intersection* ti = new TexturedRectangleIntersection;
    ti->t = i->t;
    ti->point = i->point;
    ti->object = this;
    
    delete i;
    return ti;
}



