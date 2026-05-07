#include "_bullets.h"

_bullets::_bullets()
{
    scale.x = 0.25;
    scale.y = 0.25;
    scale.z = 0.25;

    pos.x = 0.0;
    pos.y = 0.0;
    pos.z = 0.0;

    start.x = 0.0;
    start.y = 0.0;
    start.z = 0.0;

    dest.x = 0.0;
    dest.y = 0.0;
    dest.z = 0.0;

    isLive = false;
    actrigger = IDLE;

    timer = 0.0f;
    deltaT = 0.0f;
    t = 0.0f;
}

_bullets::~_bullets()
{
    delete p;
    p = nullptr;
}

void _bullets::initBlt(vec3 pPos, char* fileName, char* modelName, char* pFileName)
{
    pos = pPos;
    modelInit(fileName, modelName);
    p->initparticles(pFileName);
}

void _bullets::shoot(vec3 sPos, vec3 dPos, float time)
{
    deltaT = time;

    if (actrigger == IDLE) {
        start = sPos;
        dest = dPos;
    }

    if (isLive) {
        timer += time;

        if (timer > 0.06f) {
            pos.x = start.x + t * (dest.x - start.x);
            pos.y = start.y + t * (dest.y - start.y);
            pos.z = start.z + t * (dest.z - start.z);

            if (actrigger == ACTIVE) {
                if (t >= 1.0f) {
                    t = 0.0f;
                    actrigger = HIT;
                    isLive = false;
                }
                else {
                    t += time + 0.1f;
                }
            }

            timer = 0.0f;
        }
    }
}

void _bullets::bulletActions()
{
   switch(actrigger)
   {
       case IDLE: isLive = false; // avoid render
            break;
       case ACTIVE: isLive = true;
            break;
       case HIT: isLive = false;
             p->generateP(pos.x,pos.y,pos.z);
             p->lifetime(deltaT);
             p->drawParticle();
       break;
   }
}

bool _bullets::collision(vec3 s, vec3 d)
{
    // To Do: change radius dummy value of 0.4 to variable
    // change threshold value to a variable
  return (sqrt(pow(s.x-d.x, 2)+pow(s.y-d.y,2)+pow(s.z-d.z,2))- 0.4< 0.03);
}

void _bullets::drawBullet()
{
    if(isLive)
    {
      drawModel();
    }

}
