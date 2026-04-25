#include "_ground.h"

_ground::_ground()
{
    //ctor
    rot.x=rot.y=rot.z =0;

    // basically flattening the cube into a road
    scale.x = 30.0f;   // road width
    scale.y = 0.2f;   // road thickness
    scale.z = 30.0f;  // road length

    pos.x = 0.0f;
    pos.y = -1.0;//-2.0f;
    pos.z = 0.0f;
}

_ground::~_ground()
{
    //dtor
}
void _ground::drawModel()
{
    glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);   // white so texture shows correctly
        glTranslatef(pos.x, pos.y, pos.z);
        glRotatef(rot.x,1,0,0);
        glRotatef(rot.y,0,1,0);
        glRotatef(rot.z,0,0,1);
        glScalef(scale.x, scale.y, scale.z);

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

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint)));   // front
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));   // back
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));  // top
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(GLuint)));  // bottom
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(GLuint)));  // right
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(GLuint)));  // left

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
}

void _ground::modelInit(char* fileName)
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

void _ground::setPosition(float x, float y, float z)
{
    pos.x = x;
    pos.y = y;
    pos.z = z;
}

void _ground::moveZ(float dz)
{
    pos.z += dz;
}

