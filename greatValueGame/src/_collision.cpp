#include "_collision.h"

_collision::_collision()
{
    //ctor
}

_collision::~_collision()
{
    //dtor
}
bool _collision::checkLaneHit(int playerLane, int obstacleLane, float playerZ, float obstacleZ, float hitDistance)
{
    // Must be in same lane first
    if (playerLane != obstacleLane){
        return false;
    }

    // Then check if they are close enough in z
    return std::fabs(playerZ - obstacleZ) <= hitDistance;

}
