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

#define PI 3.14159
#define GLEW_STATIC

using namespace std;

typedef struct{
   GLdouble x;
   GLdouble y;
   GLdouble z;
}vec3;

typedef struct{
   GLdouble x;
   GLdouble y;
}vec2;

#endif // _COMMON_H
