#include <primitives.h>
#include <lodepng.h>

#include <cmath>
#include <iostream>
#include <cstdlib>

float Vector::normSq()
{
    return x*x + y*y + z*z;
}

float Vector::norm()
{
    return sqrt(normSq());
}

Vector Vector::normalize()
{
    float d = norm();
    if (d == 0) return Vector(0,0,0);
    return Vector(x / d, y / d, z / d);
}

Point Ray::operator()(float t)
{
    return origin + t * direction;
}

Point operator+(Point lhs, Vector rhs)
{
    return Point(lhs.x + rhs.x,
                 lhs.y + rhs.y,
                 lhs.z + rhs.z);
}

Point operator+(Vector lhs, Point rhs)
{
    return Point(lhs.x + rhs.x,
                 lhs.y + rhs.y,
                 lhs.z + rhs.z);
}

Vector operator-(Point lhs, Point rhs)
{
    return Vector(lhs.x - rhs.x,
                  lhs.y - rhs.y,
                  lhs.z - rhs.z);
}

Vector operator+(Vector lhs, Vector rhs)
{
    return Vector(lhs.x + rhs.x,
                  lhs.y + rhs.y,
                  lhs.z + rhs.z);
}

Vector operator-(Vector lhs, Vector rhs)
{
    return Vector(lhs.x - rhs.x,
                  lhs.y - rhs.y,
                  lhs.z - rhs.z);
}

Vector operator*(float a, Vector v)
{
    return Vector(a * v.x,
                  a * v.y,
                  a * v.z);
}

Vector operator*(Vector v, float a)
{
    return Vector(a * v.x,
                  a * v.y,
                  a * v.z);
}

float dot(Vector lhs, Vector rhs)
{
    return lhs.x * rhs.x +
           lhs.y * rhs.y +
           lhs.z * rhs.z ;
}

Vector cross(Vector lhs, Vector rhs)
{
    return Vector(lhs.y*rhs.z - lhs.z*rhs.y,
                  lhs.z*rhs.x - lhs.x*rhs.z,
                  lhs.x*rhs.y - lhs.y*rhs.x);
}

Color Color::operator*=(float a)
{
    this->r *= a;
    this->g *= a;
    this->b *= a;
    
    return *this;
}

Color Color::operator*=(Color c)
{
    this->r *= c.r;
    this->g *= c.g;
    this->b *= c.b;
    
    return *this;
}

Color Color::operator+=(Color c)
{
    this->r += c.r;
    this->g += c.g;
    this->b += c.b;
    
    return *this;
}

#define CLAMP_FLOAT(f) ((f) < 0.0 ? 0.0 : ((f) > 1.0) ? 1.0 : (f))

void Color::clampThis()
{
    this->r = CLAMP_FLOAT(r);
    this->g = CLAMP_FLOAT(g);
    this->b = CLAMP_FLOAT(b);
}

Color operator*(float a, Color c)
{
    return Color(a*c.r, a*c.g, a*c.b);
}

Color operator*(Color c, float a)
{
    return Color(a*c.r, a*c.g, a*c.b);
}

Color operator*(Color lhs, Color rhs)
{
    return Color(lhs.r * rhs.r,
                 lhs.g * rhs.g,
                 lhs.b * rhs.b);
}

Color operator+(Color lhs, Color rhs)
{
    return Color(lhs.r + rhs.r,
                 lhs.g + rhs.g,
                 lhs.b + rhs.b);
}

Texture loadTexture(const char* filename)
{
    unsigned char* buffer;
    unsigned width, height;
    
    int err = lodepng_decode24_file(&buffer, &width, &height, filename);
    if (err)
    {
        std::cerr << "could not load texture from " << filename << "\n";
        exit(1);
    }
    
    Color* image = new Color[width * height];
    
    // lodepng actually loads the image upside down.
    // they say they don't. but they do.
    for (unsigned row = 0; row < height; row++)
    {
        for (unsigned col = 0; col < width; col++)
        {
            unsigned imgIdx = (row * width + col);
            unsigned bufIdx = ((height - row - 1) * width + col) * 3;
            image[imgIdx].r = buffer[bufIdx + 0] / 255.0;
            image[imgIdx].g = buffer[bufIdx + 1] / 255.0;
            image[imgIdx].b = buffer[bufIdx + 2] / 255.0;
        }
    }
    
    free(buffer);
    
    Texture tex;
    tex.pixels = image;
    tex.width = width;
    tex.height = height;
    
    return tex;
}

Color Texture::operator()(float s, float t)
{
    int x = (int) (s * width);
    int y = (int) (t * height);
    
    // clamp values
    if (x >= width) x = width - 1;
    if (y >= height) y = height - 1;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    
    return pixels[y * width + x];
}