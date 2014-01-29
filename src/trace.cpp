#include <trace.h>
#include <cmath>

void drawScene(Scene* scene, unsigned char* buffer, int width, int height, int maxDepth, bool orthographic, int antialias)
{
    float viewWidth, viewHeight, pixWidth, pixHeight, pixWidthOverK, pixHeightOverK;
    int k = antialias + 1, d = antialias * antialias;
    
    viewWidth  = scene->viewPlaneRight - scene->viewPlaneLeft;
    viewHeight = scene->viewPlaneTop   - scene->viewPlaneBottom;
    pixWidth  = (viewWidth  / width );
    pixHeight = (viewHeight / height);
    pixWidthOverK  = pixWidth  / k;
    pixHeightOverK = pixHeight / k;
    
    float pixBottom, pixLeft;
    Point pixelLoc(0, 0, scene->viewPlaneZ);
    Point viewpoint(0, 0, 0);
    
    for (int y = 0; y < height; y++) 
    {
        pixBottom = scene->viewPlaneBottom + (pixHeight * y);
        
        for (int x = 0; x < width; x++) 
        {
            pixLeft = scene->viewPlaneLeft + (pixWidth * x);
            Color pixelColor(0,0,0);
            
            for (int i = 1; i < k; ++i)
            {
                pixelLoc.y = pixBottom + (i * pixHeightOverK);
                
                for (int j = 1; j < k; ++j)
                {
                    pixelLoc.x = pixLeft + (j * pixWidthOverK);
                    
                    Ray r;
                    r.origin = pixelLoc;
                    if (orthographic)
                        r.direction = Vector(0,0,-1);
                    else
                        r.direction = (pixelLoc - viewpoint).normalize();
                    
                    Color tempColor;
                    trace(scene, &r, maxDepth, tempColor);
                    pixelColor += tempColor;
                }
            }
            
            pixelColor.r /= d;
            pixelColor.g /= d;
            pixelColor.b /= d;
            pixelColor.clampThis();
            
            // lodepng actually wants this upside down
            int bufIdx = ((height - y - 1) * width + x) * 3;
            buffer[bufIdx + 0] = (unsigned char) (pixelColor.r * 255);
            buffer[bufIdx + 1] = (unsigned char) (pixelColor.g * 255);
            buffer[bufIdx + 2] = (unsigned char) (pixelColor.b * 255);
        }
    }
}

#define isZero(color) ((color).r == 0 && (color).g == 0 && (color).b == 0)

void trace(Scene* scene, Ray* ray, int maxDepth, Color &color)
{
    if (maxDepth <= 0)
    {
        color = scene->backgroundColor;
        return;
    }
    
    /*****************************************************
     ************* INTERSECTION CALCULATIONS *************
     *****************************************************/
    
    Intersection* intersection = findFirstIntersection(scene, ray);
    
    if (!intersection) 
    {
        // no intersection with an object, so we check to see if the ray 
        // is pointing at a light source
        
        // start with point sources since they are "closer" than directional sources
        Color closestColor;
        float closestPoint = -1;
        
        for (vector<PointLight*>::iterator it = scene->pointLights.begin(); 
             it != scene->pointLights.end(); ++it)
        {
            PointLight* light = *it;
            
            // find potential intersection between ray and point
            float t = (light->location.x - ray->origin.x) / ray->direction.x;
            
            if (t > EPSILON && t < closestPoint)
            {
                // plug back in to see if the solution is valid for all three dimensions
                float yVal = light->location.y - ray->origin.y - t * ray->direction.y;
                float zVal = light->location.z - ray->origin.z - t * ray->direction.z;
                bool yValCheck = yVal == 0;
                bool zValCheck = zVal == 0;
                
                if (yValCheck && zValCheck)
                {
                    closestPoint = t;
                    closestColor = light->color;
                }
            }
        }
        
        if (closestPoint >= 0)
        {
            color = closestColor;
            return;
        }
        
        // now check directional sources
        for (vector<DirectionalLight*>::iterator it = scene->directionalLights.begin(); 
             it != scene->directionalLights.end(); ++it)
        {
            DirectionalLight* light = *it;
            float angleCos = dot(ray->direction, light->direction);
            if (angleCos == -1) \
            {
                // ray is pointing towards directional light
                color = light->color;
                return;
            }
        }
        
        // no light sources, just return background
        color = scene->backgroundColor;
        return;
    }
    
    /*****************************************************
     *************** LIGHTING CALCULATIONS ***************
     *****************************************************/
    
    Material material;
    Vector normal;
    Point point;
    
    point = intersection->point;
    intersection->getNormal(normal);
    intersection->getMaterial(material);
    
    // initialize color to 0
    color = Color(0,0,0);
    
    bool hasDiffuse = !isZero(material.diffuse);
    bool hasSpecular = !isZero(material.specular);
    bool hasRefracted = !isZero(material.refracted);
    
    /*****************************************************
     ****************** COMPUTE AMBIENT ******************
     *****************************************************/
    
    color += material.ambient * scene->ambientLight;
    
    /*****************************************************
     ******************* COMPUTE LOCAL *******************
     *****************************************************/
    
    if (hasDiffuse || hasSpecular)
    {
        Color diffuse(0,0,0);
        Color specular(0,0,0);
        Vector toViewer = -1 * ray->direction;
        
        // point lights
        for (vector<PointLight*>::iterator it = scene->pointLights.begin();
             it != scene->pointLights.end(); ++it)
        {
            PointLight* light = *it;
            
            // first make sure object is facing light
            Vector toLight = (light->location - point).normalize();
            float angleCos = dot(normal, toLight);
            if (angleCos <= 0)
                continue;
            
            // first check for shadows
            Color shadow;
            Ray shadowRay;
            shadowRay.direction = light->location - point;
            shadowRay.origin = point;
            computeShadow(scene, &shadowRay, 1, maxDepth, shadow);
            
            if (isZero(shadow))
                // light is completely blocked
                continue;
            
            // compute diffuse reflection
            if (hasDiffuse)
            {
                diffuse += angleCos * light->color * shadow;
            }
            
            // compute specular reflection
            if (hasSpecular)
            {
                Vector idealReflect = 2 * angleCos * normal - toLight;
                float angleCos2 = dot(idealReflect, toViewer);
                if (angleCos2 > 0)
                    specular += pow(angleCos2, material.shininess) * light->color * shadow;
            }
        }
        
        diffuse *= material.diffuse;
        specular *= material.specular;
        color += diffuse + specular;
    }
    
    /*****************************************************
     ****************** COMPUTE REFLECTED ****************
     *****************************************************/
    
    if (hasSpecular)
    {
        // first compute reflection vector
        Ray reflect;
        Vector incoming = -1 * ray->direction;
        reflect.origin = point;
        reflect.direction = 2 * dot(incoming, normal) * normal - incoming;
        
        // use recursive call to determine the reflected color
        Color reflected;
        trace(scene, &reflect, maxDepth - 1, reflected);
        
        reflected *= material.specular;
        color += reflected;
    }
    
    /*****************************************************
     ***************** COMPUTE REFRACTED *****************
     *****************************************************/
    
    if (hasRefracted)
    {
        // compute refraction vector
        Ray refractRay;
        refractRay.origin = point;
        refractRay.direction = ray->direction;
        
        // use recursive call to determine the refracted color
        Color refracted;
        trace(scene, &refractRay, maxDepth - 1, refracted);
        
        refracted *= material.refracted;
        color += refracted;
    }
    
    color += material.emission;
    
    delete intersection;
}

// recursively computes how much of a shadow is being cast on a point relative to a particular light source
void computeShadow(Scene* scene, Ray* shadow, float lightT, int maxDepth, Color &result)
{
    if (maxDepth <= 0)
    {
        result = Color(1,1,1);
    }
    
    Intersection* inter = findFirstIntersection(scene, shadow);
    
    if (!inter || (lightT >= 0 && inter->t > lightT))
    {
        // no objects are between the point and the light source
        result = Color(1,1,1);
    }
    else
    {
        // there is an object between here and the light source,
        // but we need to see whether it is transparent
        Material m;
        inter->getMaterial(m);
        
        if (!isZero(m.refracted))
        {
            Ray refractRay;
            refractRay.origin = inter->point;
            refractRay.direction = shadow->direction;
            
            computeShadow(scene, &refractRay, lightT - inter->t, maxDepth - 1, result);
            result *= m.refracted;
        }
        else
            result = Color(0,0,0);
    }
    
    if (inter) delete inter;
}

Intersection* findFirstIntersection(Scene* scene, Ray* ray)
{
    vector<GeometricObject*>::iterator it;
    Intersection *intersection, *closest = NULL;
    GeometricObject* object;
    
    for (it = scene->objects.begin(); it != scene->objects.end(); ++it) 
    {
        object = *it;
        intersection = object->intersect(ray);
        
        if (intersection)
        {
            if (!closest || (intersection->t < closest->t))
            {
                if (closest) delete closest;
                closest = intersection;
            }
            else
            {
                delete intersection;
            }
        }
    }
    
    return closest;
}