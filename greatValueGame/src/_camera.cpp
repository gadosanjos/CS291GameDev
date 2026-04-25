#include "_camera.h"

_camera::_camera()
{
    //ctor
    eye.x = 0;
    eye.y = 2;
    eye.z = 10;

    dest.x = 0;
    dest.y = 0;
    dest.z = 0;

    up.x = up.z = 0;
    up.y = 1;

    step = 1.5;
    theta.x =0; // yaw
    theta.y =0; // pitch

    distance = sqrt(pow(eye.x-dest.x,2) +
                    pow(eye.y-dest.y,2) +
                    pow(eye.z-dest.z,2));

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
    // Convert to radians
    float yawRad = theta.x * (float)PI / 180.0f;
    float pitchRad = theta.y * (float)PI / 180.0f;

    // Forward direction (from yaw + pitch)
    float fx = cosf(pitchRad) * sinf(yawRad);
    float fy = sinf(pitchRad);
    float fz = -cosf(pitchRad) * cosf(yawRad);

    // Camera target = position + forward
    dest.x = eye.x + fx;
    dest.y = eye.y + fy;
    dest.z = eye.z + fz;

}

void _camera::canRotY()
{

}

void _camera::camMoveFB(int dir)
{
    // Forward vector
    float fx = dest.x - eye.x;
    float fy = dest.y - eye.y;
    float fz = dest.z - eye.z;

    // Ignore pitch so movement stays on ground
    fy = 0.0f;

    // Normalize
    float len = sqrt(fx * fx + fz * fz);
    if (len != 0.0f)
    {
        fx /= len;
        fz /= len;
    }

    // Move camera and target together
    eye.x  -= fx * step * dir;
    eye.z  -= fz * step * dir;

    dest.x -= fx * step * dir;
    dest.z -= fz * step * dir;
}

void _camera::camMoveLR(int dir)
{
    // Forward vector
    float fx = dest.x - eye.x;
    float fy = dest.y - eye.y;
    float fz = dest.z - eye.z;

    // Ignore pitch so strafing stays on ground
    fy = 0.0f;

    // Normalize forward on XZ plane
    float len = sqrt(fx * fx + fz * fz);
    if (len != 0.0f)
    {
        fx /= len;
        fz /= len;
    }

    // Right vector from forward
    float rx = -fz;
    float rz = fx;

    eye.x  += rx * step * dir;
    eye.z  += rz * step * dir;

    dest.x += rx * step * dir;
    dest.z += rz * step * dir;
}

void _camera::setUpCamera()
{
    gluLookAt(
              eye.x,eye.y,eye.z,
              dest.x,dest.y,dest.z,
              up.x,up.y,up.z
              );
}

void _camera::followTarget(vec3 targetPos, float targetYaw, float followDistance, float followHeight, float lookHeight)
{
    float yawRad = targetYaw * (float)PI / 180.0f;

    eye.x = targetPos.x - sinf(yawRad) * followDistance;
    eye.y = targetPos.y + followHeight;
    eye.z = targetPos.z + cosf(yawRad) * followDistance;

    dest.x = targetPos.x;
    dest.y = targetPos.y + lookHeight;
    dest.z = targetPos.z;
}
