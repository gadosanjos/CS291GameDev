#include "_model.h"

_model::_model()
{
    //ctor
}

_model::~_model()
{
    //dtor
}
void _model::drawModel()
{

    glPushMatrix();
        glColor3f(1.0,1.0,1.0);
        glTranslatef(pos.x,pos.y,pos.z);
        glRotatef(rot.x, 1,0,0);
        glRotatef(rot.y, 0,1,0);
        glRotatef(rot.z, 0,0,1);
        glScalef(scale.x,scale.y,scale.z);

        //  glutSolidTorus(0.5,1.5,40,40);
        myTex->BindTex();             // activate my texture
        //glutSolidTeapot(1.0);
        glutSolidCube(1.0);
    glPopMatrix();
}

void _model::initModel(char * filename)
{
    rot.x = rot.y = rot.z =0;
    pos.x = pos.y =0.0;
    pos.z =0;
    scale.x =scale.y=scale.z =1.0;

    myTex->loadTexture(filename);
}
