#ifndef _COLLISION_H
#define _COLLISION_H

#include <cmath>

class _collision
{
    public:
        _collision();
        virtual ~_collision();


        bool checkLaneHit(int playerLane, int obstacleLane, float playerZ, float obstacleZ, float hitDistance);


    protected:

    private:
};

#endif // _COLLISION_H
