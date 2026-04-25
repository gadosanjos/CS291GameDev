#include "_player.h"
#include <cmath>

_Player::_Player()
{
    pos.x = 0.0;
    pos.y = 0.0;
    pos.z = 0.0;
    yaw = 0.0f;
    moveSpeed = 8.0f;
    turnSpeed = 120.0f;
}

_Player::~_Player()
{
    delete model;
}

void _Player::init(const char* modelFile, char* textureFile)
{
    model->initModel(modelFile, textureFile);
    model->actionTrigger = model->STAND;
}

void _Player::update(float deltaT)
{
    if (turnLeft) {
        yaw += turnSpeed * deltaT;
    }

    if (turnRight) {
        yaw -= turnSpeed * deltaT;
    }

    float yawRad = yaw * (float)PI / 180.0f;

    float forwardX = sinf(yawRad);
    float forwardZ = -cosf(yawRad);

    if (moveForward) {
        pos.x += forwardX * moveSpeed * deltaT;
        pos.z += forwardZ * moveSpeed * deltaT;
    }

    if (moveBackward) {
        pos.x -= forwardX * moveSpeed * deltaT;
        pos.z -= forwardZ * moveSpeed * deltaT;
    }

    if (moveForward || moveBackward) {
        model->actionTrigger = model->RUN;
    } else {
        model->actionTrigger = model->STAND;
    }
}

void _Player::draw()
{
    glPushMatrix();
        glTranslatef((GLfloat)pos.x, (GLfloat)pos.y, (GLfloat)pos.z);
        glRotatef(-yaw, 0, 1, 0);
        glRotatef(-90, 1, 0, 0);
        glRotatef(90, 0, 0, 1);
        glScalef(0.03f, 0.03f, 0.03f);

        model->actions();
        model->Draw();
    glPopMatrix();
}

void _Player::setMoveForward(bool value)
{
    moveForward = value;
}

void _Player::setMoveBackward(bool value)
{
    moveBackward = value;
}

void _Player::setTurnLeft(bool value)
{
    turnLeft = value;
}

void _Player::setTurnRight(bool value)
{
    turnRight = value;
}
