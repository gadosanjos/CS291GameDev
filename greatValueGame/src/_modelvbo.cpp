#include "_modelvbo.h"

_modelVBO::_modelVBO()
{
    //ctor
    rot.x=rot.y=rot.z =0;
    scale.x=scale.y=scale.z=1.0;
    pos.x=pos.y=0;
    pos.z =0;
}

_modelVBO::~_modelVBO()
{
    //dtor
}
void _modelVBO::drawModel()
{
    glPushMatrix();
        glColor3f(1,1,1);
        glTranslatef(pos.x,pos.y,pos.z);
        glRotatef(rot.x,1,0,0);
        glRotatef(rot.y,0,1,0);
        glRotatef(rot.z,0,0,1);

        glScalef(scale.x,scale.y,scale.z);

        myTex->BindTex();

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER,vboTex);
        glTexCoordPointer(2,GL_FLOAT,0,(void*)0);

        glBindBuffer(GL_ARRAY_BUFFER,vboNorm);
        glNormalPointer(GL_FLOAT,0,0);

        glBindBuffer(GL_ARRAY_BUFFER,vboVert);
        glVertexPointer(3,GL_FLOAT,0,0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eboID);

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint)));   // front
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));   // back
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));  // top
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(GLuint)));  // bottom
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(GLuint)));  // right
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(GLuint)));  // left

        // or just
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
}

void _modelVBO::modelInit(char* fileName)
{
    myTex->loadTexture(fileName);

    //vertex Buffer
    glGenBuffers(1,&vboVert);
    glBindBuffer(GL_ARRAY_BUFFER,vboVert);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

     //Normal Buffer
    glGenBuffers(1,&vboNorm);
    glBindBuffer(GL_ARRAY_BUFFER,vboNorm);
    glBufferData(GL_ARRAY_BUFFER,sizeof(normals),normals,GL_STATIC_DRAW);

     //texture Buffer
    glGenBuffers(1,&vboTex);
    glBindBuffer(GL_ARRAY_BUFFER,vboTex);
    glBufferData(GL_ARRAY_BUFFER,sizeof(texCoords),texCoords,GL_STATIC_DRAW);

    //index buffer
    glGenBuffers(1,&eboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
}
