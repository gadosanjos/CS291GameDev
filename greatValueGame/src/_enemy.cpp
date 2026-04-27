#include "_enemy.h"
#include <cmath>

_Enemy::_Enemy()
{
    pos.x = 0.0;
    pos.y = 0.0;
    pos.z = 0.0;
    yaw = 0.0f;
    moveSpeed = 6.0f;
    active = true;
}

_Enemy::~_Enemy()
{
    delete model;
}

void _Enemy::init(const char* modelFile, char* textureFile)
{
    model->initModel(modelFile, textureFile);
    model->actionTrigger = model->RUN;
}

void _Enemy::update(float deltaT, const vec3& playerPos)
{
    if (!active) return;

    float dx = (float)(playerPos.x - pos.x);
    float dz = (float)(playerPos.z - pos.z);

    float dist = sqrtf(dx * dx + dz * dz);

    // only chase if not already extremely close
    if (dist > 1.5f)
    {
        dx /= dist;
        dz /= dist;

        yaw = atan2f(dx, -dz) * 180.0f / (float)PI;

        // move toward player if within chase range
        if (dist < 25.0f)
        {
            pos.x += dx * moveSpeed * deltaT;
            pos.z += dz * moveSpeed * deltaT;
        }
    }
}

void _Enemy::draw()
{
    if (!active) return;

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
