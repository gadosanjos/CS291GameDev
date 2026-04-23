#include "_bullets.h"

_bullets::_bullets()
{
    //ctor

    bpos.x = 0;
    bpos.y = -0.4;
    bpos.z =-7;

    brot.x =0;
    brot.y = 0;
    brot.z =0;

    bscale.x = 0.2;
    bscale.y = 0.2;
    bscale.z = 1.0;
}

_bullets::~_bullets()
{
    //dtor
}
void _bullets::initBlt(int x, int y, char* fileName)
{
    /*initQuad(fileName);
    xFrames =x;
    yFrames =y;

    xMin =0;
    xMax =1.0/(float)xFrames;
    yMax =1.0/(float)yFrames;
    yMin =0;
    */
}

void _bullets::update(vec3 pos)
{

}

void _bullets::shoot(vec3 spos, vec3 dpos, float deltaT)
{
   if(actionTrigger==IDLE)
   {
       dest= dpos;

       // angle between two point
       // arccos(dot product of the points

       float X = -spos.x + dest.x;
       float Y = -spos.y + dest.y;

       dest.x>=0?brot.z =(atan(Y/X)*180/PI)+90:brot.z = ((atan(Y/X)*180/PI)-90);
   }

   if(isLive)
   {
       timer +=deltaT;
       if(timer>0.06)
       {

         bpos.x = spos.x +t*(dest.x-spos.x);
         bpos.y = spos.y +t*(dest.y-spos.y);
         bpos.z = spos.z +t*(dest.z-spos.z);

         if(actionTrigger == ACTIVE)
        {
           if(t>=1) {t=0; actionTrigger = IDLE;} //reset the bullet
           else t+= (deltaT+0.1);  // let buulet move on the path
        }

       timer =0;
       }
   }
}



void _bullets::bulletActions()
{
  switch(actionTrigger)
    {
        case IDLE: isLive = false;

             bpos.x =0;
             bpos.y =-4;
             bpos.z =-7;

        break; // if bullet is in the storage
        case ACTIVE: isLive= true;break; // bullet is on the move
        case HIT: isLive = false; break; // when bullet hit the target or move out of bounce
        default: break;
    }
}

void _bullets::drawBlt()
{
   if(isLive)
    {
        glDisable(GL_TEXTURE_2D);
        glPushMatrix();
        glTranslatef(bpos.x,bpos.y,bpos.z);
        glutSolidTeapot(0.5);
        glPopMatrix();
        glEnable(GL_TEXTURE_2D);
 //  updateQuad();
 //  drawQuad();
   }
}
