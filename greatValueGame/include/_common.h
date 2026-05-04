#ifndef _COMMON_H
#define _COMMON_H

#include<string>
#include<GL/glew.h>
#include <windows.h>
#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <gl/gl.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>
#include <chrono>

#include <vector>

#include <fstream>
#include <sstream>

#define PI 3.14159
#define GLEW_STATIC

#define MAX_PARTICLES 5000

using namespace std;

typedef struct{
   double x;
   double y;
   double z;
}vec3;

// --- Data Structures ---
struct vec4 { float x, y, z, w; };
struct vec2 { float u, v; };
struct vec2f { double x, y; };
struct faces { int v, t, n; }; // Stores indices for Position, Texture, and Normal

#endif // _COMMON_H
