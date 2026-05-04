#include "_spritesheet.h"

_spritesheet::_spritesheet()
{
    //ctor
    xMin = yMin = 0.0;
    xMax = yMax = 1;

    pos.x = pos.y = 0;
    pos.z = -8;

    vertex[0].x = -1.0; vertex[0].y = -1.0; vertex[0].z = 1.0;
    vertex[1].x =  1.0; vertex[1].y = -1.0; vertex[1].z = 1.0;
    vertex[2].x =  1.0; vertex[2].y =  1.0; vertex[2].z = 1.0;
    vertex[3].x = -1.0; vertex[3].y =  1.0; vertex[3].z = 1.0;

    scale.x = scale.y = 1;
    scale.z = 1;
}

_spritesheet::~_spritesheet()
{
    //dtor
    delete sprite;
}

void _spritesheet::spriteInit(char* fileName, int xFrames, int yFrames)
{
    sprite->loadTexture(fileName);
    frames.u = xFrames;
    frames.v = yFrames;
}

void _spritesheet::drawSprite()
{
    glPushMatrix();

        glTranslatef(pos.x,pos.y,pos.z);
        glRotatef(rot.x,1,0,0);
        glRotatef(rot.y,0,1,0);
        glRotatef(rot.z,0,0,1);
        glScalef(scale.x,scale.y,scale.z);

        sprite->BindTex();

        glBegin(GL_QUADS);
            glTexCoord2f(xMin,yMax);
            glVertex3f(vertex[0].x,vertex[0].y,vertex[0].z);
            glTexCoord2f(xMax,yMax);
            glVertex3f(vertex[1].x,vertex[1].y,vertex[1].z);
            glTexCoord2f(xMax,yMin);
            glVertex3f(vertex[2].x,vertex[2].y,vertex[2].z);
            glTexCoord2f(xMin,yMin);
            glVertex3f(vertex[3].x,vertex[3].y,vertex[3].z);
        glEnd();

    glPopMatrix();
}

void _spritesheet::spriteActions(float deltaT)
{
    timer+=deltaT;

    // TO FIGURE OUT POSITION 0,0 is top left 1,1 bottom right


    switch(actionTrigger)
    {
    case STAND:
        xMin = 0;
        yMin = 2.0/frames.v;
        xMax = 1.0/frames.u;
        yMax = 3.0/frames.v;
        break;
    case WALKLEFT:
        if (timer > 0.08f) {
            xMin += 1.0f / frames.u;

            if (xMin >= 1.0f) {
                xMin = 0.0f;
            }

            xMax = xMin + (1.0f / frames.u);
            yMin = 0.0f;
            yMax = 1.0f;

            timer = 0.0f;
        }
        break;
    case WALKRIGHT:
        if(timer > 0.08){
            xMin += 1.0/frames.u;
            yMin = 3/frames.v;
            xMax += 1.0/frames.u;
            yMax = 1.0;
            timer = 0;
        }
        break;
    case WALKFRONT:
        if(timer > 2){
            xMin += 1.0/frames.u;
            yMin = 0;
            xMax += 1.0/frames.u;
            yMax = 1.0/frames.v;
            timer = 0;
        }
        break;
    case WALKBACK:
        if(timer > 0.08){
            xMin += 1.0/frames.u;
            yMin = 2.0/frames.v;
            xMax += 1.0/frames.u;
            yMax = 3.0/frames.v;
            timer = 0;
        }
        break;
    default:
        break;

    }
}
