#ifndef _PARTICLES_H
#define _PARTICLES_H

#include<_common.h>
#include<_quad.h>

class _particles: public _quad
{
    public:
        _particles();
        virtual ~_particles();

        int numDrops =0;
        int newDrops =0;
        float timer;

        typedef struct{
            bool isLive;  // check for life time
            vec3 pos;     // position of the particle
            vec3 dir;     // direction of the particle
            vec3 color;   // material color
            vec3 vel;     // velosity
            float alpha;  // determinde life time of the particle
            float mass;   // for physics
            float radius; // for explosions
            float expRadius;
            float theta;  // angle
            float t;
        }particle;

        particle drops[MAX_PARTICLES];

        void drawParticle();  // render particles
        void lifetime(float); // update particles
        void generateP(float,float,float);// generate particles
        void initparticles(char*); // initialize particles

    protected:

    private:
};

#endif // _PARTICLES_H
