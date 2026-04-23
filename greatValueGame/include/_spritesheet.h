#ifndef _SPRITESHEET_H
#define _SPRITESHEET_H

#include "_common.h"
#include "_texloader.h"

class _spritesheet
{
    public:
        _spritesheet();
        virtual ~_spritesheet();

        _texLoader* sprite = new _texLoader();

        vec3 pos;
        vec3 rot;
        vec3 scale;

        vec2 frames;

        vec3 vertex[4];

        float timer = 0;

        enum{STAND,WALKLEFT,WALKRIGHT,WALKFRONT,WALKBACK};

        float xMin, xMax, yMin, yMax;

        int actionTrigger = 0;

        void spriteInit(char*,int,int); //filename xframes yframes
        void drawSprite();
        void spriteActions(float);

    protected:

    private:
};


#endif // _SPRITESHEET_H
