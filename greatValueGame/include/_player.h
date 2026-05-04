#ifndef _PLAYER_H
#define _PLAYER_H

#include "_common.h"
#include "_ModelLoaderMD2.h"

class _Player
{
public:
    _Player();
    virtual ~_Player();

    void init(const char* modelFile, char* textureFile,const char* modelFileW, char* textureFileW);
    void update(float deltaT);
    void draw();

    // input setters
    void setMoveForward(bool value);
    void setMoveBackward(bool value);
    void setTurnLeft(bool value);
    void setTurnRight(bool value);
    void setAttack(bool value);
    void setDeath(bool value);

    // Movement
    vec3 pos;
    float yaw = 0.0f;
    float moveSpeed = 8.0f;
    float turnSpeed = 120.0f;

    // State
    int playerLives = 5;
    enum STATE {IDLE,RUN ,ATTACK, JUMP};
    float autoFireTimer = 0.0f;
    float autoFireRate = 1.0f;
    float autoFireRange = 25.0f;
    int bulletDamage = 1;

    _ModelLoaderMD2* model = new _ModelLoaderMD2();
    _ModelLoaderMD2* modelWeapon = new _ModelLoaderMD2(); // waepon

private:
    bool dead = false;
    bool attack = false;
    bool moveForward = false;
    bool moveBackward = false;
    bool turnLeft = false;
    bool turnRight = false;
};

#endif // _PLAYER_H
