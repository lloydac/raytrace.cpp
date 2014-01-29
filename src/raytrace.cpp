#include <trace.h>
#include <lodepng.h>
#include <iostream>

/*************************************************
 *************** DRAWING PARAMETERS **************
 *************************************************/

// controls the number of rays sent out per pixel.
// the number of rays is antialiasingFactor^2, so
// a value of 1 means no antialiasing takes place
int antialiasingFactor = 3;
// enables orthographic viewing
bool orthographic = false;
// controls the number of times the ray tracer recurses
int recursionDepth = 5;
// output image dimensions in pixels
int width = 1024;
int height = 1024;
// the name of the output file
const char* outputFile = "raytrace.png";

/* local functions */
Scene* createScene();

int main(int argc, char** argv)
{
    std::cout << "creating scene...\n";
    Scene* scene = createScene();
    unsigned char* canvas = new unsigned char[width * height * 3];
    
    std::cout << "drawing scene...\n";
    drawScene(scene, canvas, width, height, recursionDepth, orthographic, antialiasingFactor);
    
    std::cout << "writing scene to file...\n";
    lodepng_encode24_file(outputFile, canvas, width, height);
}

#define Z (-20)

Scene* createScene() {
    Scene* scene = new Scene;
    
    // set up viewing parameters
    scene->viewPlaneTop = 10;
    scene->viewPlaneBottom = -10;
    scene->viewPlaneLeft = -10;
    scene->viewPlaneRight = 10;
    scene->viewPlaneZ = Z;
    
    // set up lighting
    scene->backgroundColor = Color(0,0,0);
    
    scene->ambientLight = Color(0.2,0.2,0.2);
    
    float emissionScale = 0.65;
    
    // light sphere 1
    PointLight* light = new PointLight;
    light->color = Color(0.5,0.5,0.5);
    light->location = Point(9,0,Z-10);
    scene->pointLights.push_back(light);
    
    Material lightBallMat;
    lightBallMat.ambient = Color(0,0,0);
    lightBallMat.diffuse = Color(0,0,0);
    lightBallMat.specular = Color(0.2,0.2,0.2);
    lightBallMat.refracted = Color(0.8,0.8,0.8);
    lightBallMat.emission = emissionScale * light->color;
    lightBallMat.shininess = 1000;
    
    Sphere* lightBall = new Sphere(lightBallMat, light->location, 0.5);
    scene->objects.push_back(lightBall);
    
    // light sphere 2
    light = new PointLight;
    light->color = Color(0.5,0.5,0.5);
    light->location = Point(-9,0,Z-10);
    scene->pointLights.push_back(light);
    
    lightBallMat.emission = emissionScale * light->color;
    lightBall = new Sphere(lightBallMat, light->location, 0.5);
    scene->objects.push_back(lightBall);
    
    // light sphere 3
    light = new PointLight;
    light->color = Color(0.5,0.5,0.5);
    light->location = Point(0,9,Z-15);
    scene->pointLights.push_back(light);
    
    lightBallMat.emission = emissionScale * light->color;
    lightBall = new Sphere(lightBallMat, light->location, 0.5);
    scene->objects.push_back(lightBall);
    
    // set up objects
    Material sphereMat;
    sphereMat.ambient = Color(1,0,0);
    sphereMat.diffuse = Color(1,0,0);
    sphereMat.specular = Color(0.2,0.2,0.2);
    sphereMat.refracted = Color(0,0,0);
    sphereMat.emission = Color(0,0,0);
    sphereMat.shininess = 3;
    
    Sphere* sphere = new Sphere(sphereMat, Point(1,-8,Z-5), 2);
    scene->objects.push_back(sphere);
    
    sphereMat.ambient = Color(0.01, 0.29, 0.7);
    sphereMat.diffuse = Color(0.01, 0.29, 0.7);
    sphere = new Sphere(sphereMat, Point(-2,-3,Z-13), 4);
    scene->objects.push_back(sphere);
    
    // now the transparent table
    Material glass;
    glass.ambient = Color(0.1,0.1,0.1);
    glass.diffuse = Color(0.1,0.1,0.1);
    glass.specular =  Color(0.35,0.35,0.35);
    glass.refracted =  Color(0.65,0.65,0.65);
    glass.emission = Color(0,0,0);
    glass.shininess = 1000;
    
    //top
    Rectangle* rect = new Rectangle(glass, 2, -6, -7, -7, Z-9, Z-17, Vector(0,1,0));
    scene->objects.push_back(rect);
    
    //front
    rect = new Rectangle(glass, 2, -6, -7, -10, Z-9, Z-9, Vector(0,0,1));
    scene->objects.push_back(rect);
    
    //back
    rect = new Rectangle(glass, 2, -6, -7, -10, Z-17, Z-17, Vector(0,0,-1));
    scene->objects.push_back(rect);
    
    //right
    rect = new Rectangle(glass, 2, 2, -7, -10, Z-9, Z-17, Vector(1,0,0));
    scene->objects.push_back(rect);
    
    //left
    rect = new Rectangle(glass, -6, -6, -7, -10, Z-9, Z-17, Vector(-1,0,0));
    scene->objects.push_back(rect);
    
    // set up walls
    Material wallMat;
    wallMat.ambient = Color(0,0.7,0.7);
    wallMat.diffuse = Color(0,0.7,0.7);
    wallMat.specular = Color(0,0,0);
    wallMat.refracted = Color(0,0,0);
    wallMat.emission = Color(0,0,0);
    wallMat.shininess = 10;
    
    Material mirrorMat;
    mirrorMat.ambient = Color(0,0,0);
    mirrorMat.diffuse = Color(0,0,0);
    mirrorMat.specular = Color(1,1,1);
    mirrorMat.refracted = Color(0,0,0);
    mirrorMat.emission = Color(0,0,0);
    mirrorMat.shininess = 1000;
    
    Plane* top = new Plane(wallMat, Point(0,scene->viewPlaneTop,0), Vector(0,-1,0));
    scene->objects.push_back(top);
    
    Plane* bottom = new Plane(wallMat, Point(0,scene->viewPlaneBottom,0), Vector(0,1,0));
    scene->objects.push_back(bottom);
    
    Plane* left = new Plane(wallMat, Point(scene->viewPlaneLeft,0,0), Vector(1,0,0));
    scene->objects.push_back(left);
    
    Plane* right = new Plane(wallMat, Point(scene->viewPlaneRight,0,0), Vector(-1,0,0));
    scene->objects.push_back(right);
    
    Plane* back = new Plane(wallMat, Point(0,0,Z-20), Vector(0,0,1));
    scene->objects.push_back(back);
    
    // set up starcraft pictures on wall
    float height = 8;
    float width = 5;
    float zVal = Z - 20 + EPSILON;
    
    Material pictureMat;
    pictureMat.ambient = Color(1,1,1);
    pictureMat.diffuse = Color(1,1,1);
    pictureMat.specular = Color(0,0,0);
    pictureMat.refracted = Color(0,0,0);
    pictureMat.emission = Color(0,0,0);
    pictureMat.shininess = 10;
    
    Texture tex1, tex2, tex3;
    tex1 = loadTexture("textures/texture1.png");
    tex2 = loadTexture("textures/texture2.png");
    tex3 = loadTexture("textures/texture3.png");
    
    TexturedRectangle *protoss, *zerg, *terran;
    protoss = new TexturedRectangle(pictureMat, tex1, XAXIS, YAXIS,
        -9 + width, -9, 0 + height, 0, zVal, zVal, Vector(0,0,1));
    zerg = new TexturedRectangle(pictureMat, tex2, XAXIS, YAXIS,
        -2.5 + width, -2.5, 0 + height, 0, zVal, zVal, Vector(0,0,1));
    terran = new TexturedRectangle(pictureMat, tex3, XAXIS, YAXIS,
        4 + width, 4, 0 + height, 0, zVal, zVal, Vector(0,0,1));
    
    scene->objects.push_back(protoss);
    scene->objects.push_back(zerg);
    scene->objects.push_back(terran);
    
    return scene;
}
