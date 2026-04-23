#ifndef _BULLETS_H
#define _BULLETS_H

#include<_common.h>

class _bullets
{
    public:
        _bullets();
        virtual ~_bullets();

        bool isLive =false;  // check the bullet active or not
        int actionTrigger=0;
        float timer;
        float t=0; // fraction of distance in parametric equation

        enum {IDLE,ACTIVE,HIT};

        void initBlt(int x, int y, char *fileName);
        void update(vec3 pos);  // update bullet movements
        void shoot(vec3,vec3,float);  // source and destination
        void bulletActions();   // Address the bullet actions
        void drawBlt();         // render bullets

        vec3 dest; // to keep track of first destination of the bullet
        vec3  bpos, brot, bscale;

    protected:

    private:
};

#endif // _BULLETS_H
