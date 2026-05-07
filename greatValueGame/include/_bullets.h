#ifndef _BULLETS_H
#define _BULLETS_H

#include <_common.h>
#include <_modelvbo.h>
#include <_particles.h>

class _bullets : public _modelVBO
{
    public:
        _bullets();
        virtual ~_bullets();

        bool isLive = false;
        int actrigger =0;
        float timer;
        float deltaT=0;
        float t =0; //parametric factor

        float explosionTimer = 0.0f;
        float explosionDuration = 0.6f;
        bool explosionGenerated = false;

        vec3 dest; // destination of the bullet move
        vec3 start;

        enum {IDLE, ACTIVE,HIT};

        void initBlt(vec3,char *, char*,char*);
        void shoot(vec3,vec3,float);
        void bulletActions();
        void drawBullet();

        bool collision(vec3, vec3);

        _particles *p = new _particles();

    protected:

    private:
};

#endif // _BULLETS_H
