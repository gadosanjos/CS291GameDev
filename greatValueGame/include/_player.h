#ifndef _PLAYER_H
#define _PLAYER_H

#include "_common.h"
#include "_ModelLoaderMD2.h"

class _Player
{
public:
    _Player();
    virtual ~_Player();

    void init(const char* modelFile, char* textureFile);
    void update(float deltaT);
    void draw();

    // input setters
    void setMoveForward(bool value);
    void setMoveBackward(bool value);
    void setTurnLeft(bool value);
    void setTurnRight(bool value);

    // Movement
    vec3 pos;
    float yaw = 0.0f;
    float moveSpeed = 8.0f;
    float turnSpeed = 120.0f;

    // State
    int playerLives = 5;
    enum STATE {IDLE,RUN ,ATTACK, JUMP};

    _ModelLoaderMD2* model = new _ModelLoaderMD2();

private:
    bool moveForward = false;
    bool moveBackward = false;
    bool turnLeft = false;
    bool turnRight = false;
};

#endif // _PLAYER_H
