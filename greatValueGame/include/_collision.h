#ifndef _COLLISION_H
#define _COLLISION_H

#include "_common.h"
#include <cmath>

class _collision
{
    public:
        _collision();
        virtual ~_collision();

        bool checkRadiusHit(const vec3& a, float radiusA, const vec3& b, float radiusB);
        bool checkLaneHit(int playerLane, int obstacleLane, float playerZ, float obstacleZ, float hitDistance);


    protected:

    private:
};

#endif // _COLLISION_H
