#include "_inputs.h"

_inputs::_inputs()
{
    //ctor
     isRotate = isTranslate = false;

     prev_mX = prev_mY =0;
}

_inputs::~_inputs()
{
    //dtor

}
void _inputs::keyPressed(_model* mdl)
{
    switch(wParam)
    {
        case VK_LEFT:
            mdl->rot.y +=1.0;
        break;

        case VK_RIGHT:
            mdl->rot.y -=1.0;
        break;

        case VK_UP:
            mdl->rot.x +=1.0;
        break;

        case VK_DOWN:
             mdl->rot.x +=1.0;
        break;

        default: break;

    }

}

void _inputs::keyPressed(_modelVBO* mdl)
{
     switch(wParam)
    {
        case VK_LEFT:
            mdl->rot.y +=1.0;
        break;

        case VK_RIGHT:
            mdl->rot.y -=1.0;
        break;

        case VK_UP:
            mdl->rot.x +=1.0;
        break;

        case VK_DOWN:
             mdl->rot.x +=1.0;
        break;

        default: break;

    }
}

void _inputs::keyPressed(_skyBox* sky)
{
      switch(wParam)
    {
        case VK_LEFT:
            sky->rot.y +=1.0;
        break;

        case VK_RIGHT:
            sky->rot.y -=1.0;
        break;

        case VK_UP:
            sky->rot.x -=1.0;
        break;

        case VK_DOWN:
             sky->rot.x +=1.0;
        break;

        default: break;
  }
}

void _inputs::keyPressed(_camera* cam)
{
    // CAMERA IS FIXED COMMENT OUT FOR DEBUGGING
     switch(wParam)
    {
        case 65:
            cam->camMoveLR(-1);
        break;

        case 68:
            cam->camMoveLR(1);
        break;

        case 87:
             cam->camMoveFB(-1);
        break;

        case 83:
             cam->camMoveFB(1);
        break;

        default: break;

    }
}

void _inputs::keyPressed(_spritesheet* mdl)
{
     switch(wParam)
    {
        case VK_LEFT:
            mdl->actionTrigger= mdl->WALKLEFT;
        break;

        case VK_RIGHT:
             mdl->actionTrigger= mdl->WALKRIGHT;
        break;

        case VK_UP:
            mdl->actionTrigger= mdl->WALKBACK;
        break;

        case VK_DOWN:
              mdl->actionTrigger= mdl->WALKFRONT;
        break;;

        default: break;

    }
}

void _inputs::keyPressed(_ModelLoaderMD2* mdl, _ModelLoaderMD2* mdlW)
{
      switch(wParam)
    {
        case VK_LEFT:
              mdlW->actionTrigger=mdl->actionTrigger=mdl->RUN;
             break;
        case VK_RIGHT:
             mdlW->actionTrigger= mdl->actionTrigger=mdl->RUN;
             break;
        case VK_UP:
             mdlW->actionTrigger= mdl->actionTrigger=mdl->JUMP;
             break;
        case VK_DOWN:
             mdlW->actionTrigger= mdl->actionTrigger=mdl->ATTACK;
             break;
        default:
            mdlW->actionTrigger=mdl->actionTrigger=mdl->RUN;  break;
    }
}


void _inputs::mouseMove(_modelVBO* mdl, double x, double y)
{

    if(isRotate){
     mdl->rot.y +=(x-prev_mX)/3.0;
     mdl->rot.x +=(y-prev_mY)/3.0;
    }
   if(isTranslate)
    {
        mdl->pos.x += (x-prev_mX)/100.0;
        mdl->pos.y += (prev_mY -y)/100.0;
    }

    prev_mX =x;
    prev_mY =y;
}

void _inputs::mouseWheel(_modelVBO* mdl, double delta)
{
   mdl->pos.z +=delta/100.0;
}


void _inputs::keyUp()
{
    switch(wParam)
    {
        default: break;
    }
}

void _inputs::mouseEvenDown( double x, double y)
{

   prev_mX =x;
   prev_mY =y;

   switch(wParam)
   {
   case MK_LBUTTON:
       isRotate = true;
       break;

   case MK_RBUTTON:
       isTranslate =true;
   case MK_MBUTTON: break;
   default: break;

   }
}

void _inputs::mouseUp()
{
     isRotate = isTranslate = false;
}

void _inputs::mouseMove(_model*mdl, double x, double y)
{

    if(isRotate){
     mdl->rot.y +=(x-prev_mX)/3.0;
     mdl->rot.x +=(y-prev_mY)/3.0;
    }
   if(isTranslate)
    {
        mdl->pos.x += (x-prev_mX)/100.0;
        mdl->pos.y += (prev_mY -y)/100.0;
    }

    prev_mX =x;  // this update will affect other mouse move functions
    prev_mY =y;
}

void _inputs::mouseMove(_skyBox* sky, double x, double y)
{
     sky->rot.y +=(x-prev_mX)/3.0;
     sky->rot.x +=(y-prev_mY)/3.0;
}


void _inputs::mouseWheel(_model* mdl, double delta)
{
    mdl->pos.z +=delta/100.0;
}

void _inputs::mouseWheel(_camera* camera, double delta)
{
    // can zoom in between values of [2, 10]
    float newZ = camera->eye.z - delta / 100.0f;

    if (newZ >= (camera->dest.z + 2.0f) && newZ <= (camera->dest.z + 10.0f)) {
        camera->eye.z = newZ;
    }
}

