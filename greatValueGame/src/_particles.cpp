#include "_particles.h"

_particles::_particles()
{
    //ctor
}

_particles::~_particles()
{
    //dtor
}

void _particles::drawParticle()
{
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   for(int i =0; i<=numDrops; i++)
   {
       if(drops[i].isLive)
       {
           pos.x = drops[i].pos.x;
           pos.y = drops[i].pos.y;
           pos.z = drops[i].pos.z;

           scale.x =0.5;
           scale.y =0.5;
           scale.z =1.0;

           drawQuad();
       }
   }
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ZERO);
}

void _particles::lifetime(float deltaT)
{
    timer +=deltaT;

    if(timer>0.06)  // animation timer
    {
        for(int i =0; i<numDrops; i++) //for all the particles
        {
            if(drops[i].isLive)
            {
                drops[i].pos.x +=drops[i].vel.x;
                drops[i].pos.y +=drops[i].vel.y;

                drops[i].alpha -=3.0;
                drops[i].t +=deltaT;

                if(drops[i].alpha<0)
                {
                    drops[i].isLive=false;
                    drops[i].t =0;
                }
            }
        }
        timer =0;
    }

}

void _particles::generateP(float mx, float my, float mz)
{
    newDrops =rand()%30+1;

    if(newDrops+numDrops > MAX_PARTICLES)
    newDrops = MAX_PARTICLES - numDrops;

    for(int i =numDrops; i<(numDrops+newDrops); i++)
    {
        drops[i].isLive = true;
        drops[i].radius = 1.0;
        drops[i].pos.x = mx;
        drops[i].pos.y = my;
        drops[i].pos.z = mz;

        drops[i].expRadius = 0.1;
        drops[i].theta = rand()%360;

        drops[i].alpha = rand()%30;
        drops[i].t =0;

        drops[i].vel.x = sin(drops[i].theta)*drops[i].expRadius;
        drops[i].vel.y = cos(drops[i].theta)*drops[i].expRadius;
        drops[i].vel.z = cos(drops[i].theta)*drops[i].expRadius;
    }

    numDrops += newDrops;
    numDrops>=MAX_PARTICLES? numDrops=0:NULL;
}

void _particles::initparticles(char* flieName)
{
    initQuad(flieName);  // calling initialization
}
