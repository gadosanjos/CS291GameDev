#include "_collision.h"

_collision::_collision()
{
    //ctor
}

_collision::~_collision()
{
    //dtor
}

bool _collision::checkRadiusHit(const vec3& a, float radiusA, const vec3& b, float radiusB)
{
    float dx = (float)(a.x - b.x);
    float dz = (float)(a.z - b.z);

    float distSq = dx * dx + dz * dz;
    float radiusSum = radiusA + radiusB;

    return distSq <= (radiusSum * radiusSum);
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
