#include "_camera.h"

_camera::_camera()
{
    //ctor

    eye.x = 0;
    eye.y = 2;
    eye.z = 10;
    des.x =des.y=des.z =0;
    up.x =up.z =0; up.y=1;

    step=0.5;

    theta.x =0;
    theta.y =0;

    distance = sqrt(pow(eye.x-des.x,2)+pow(eye.y-des.y,2)+pow(eye.z-des.z,2));

}

_camera::~_camera()
{
    //dtor
}
void _camera::camReset()
{

}

void _camera::camRotX()
{

}

void _camera::canRotY()
{

}

void _camera::camMoveFB(int dir)
{
    eye.z += step*dir; //left  dir= -1 and right is dir=1
    des.z += step*dir; //left  dir= -1 and right is dir=1
}

void _camera::camMoveLR(int dir)
{
    eye.x += step*dir; //left  dir= -1 and right is dir=1
    des.x += step*dir; //left  dir= -1 and right is dir=1
}

void _camera::setUpCamera()
{
    gluLookAt(eye.x,eye.y,eye.z,
              des.x,des.y,des.z,
              up.x,up.y,up.z);
}
