#ifndef _ENEMY_H
#define _ENEMY_H

#include "_common.h"
#include "_ModelLoaderMD2.h"

class _Enemy
{
public:
    _Enemy();
    virtual ~_Enemy();

    void init(const char* modelFile, char* textureFile,const char* modelFileW, char* textureFileW);
    void update(float deltaT, const vec3& playerPos);
    void draw();

    int health = 3;
    bool dead = false;

    void setHealth(int amount);
    void takeDamage(int amount);

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
    _ModelLoaderMD2* modelWeapon = new _ModelLoaderMD2(); // waepon

};

#endif // _ENEMY_H
