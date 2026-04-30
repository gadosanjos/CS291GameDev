#ifndef _ENEMY_H
#define _ENEMY_H

#include "_common.h"
#include "_ModelLoaderMD2.h"

class _Enemy
{
public:
    _Enemy();
    virtual ~_Enemy();

    void init(const char* modelFile, char* textureFile);
    void update(float deltaT, const vec3& playerPos);
    void draw();

    enum EnemyState
    {
        IDLE,
        CHASE
    };

    EnemyState state = IDLE;

    vec3 pos;
    float yaw = 0.0f;
    float moveSpeed = 3.0f;
    bool active = true;

    float startChaseDistance = 25.0f;
    float stopChaseDistance = 40.0f;
    float stopNearPlayerDistance = 1.5f;

    _ModelLoaderMD2* model = new _ModelLoaderMD2();
};

#endif // _ENEMY_H
