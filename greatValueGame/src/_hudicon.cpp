#include "_hudIcon.h"

_hudIcon::_hudIcon()
{
    pos.x = 50.0f;
    pos.y = 50.0f;
    pos.z = 0.0f;
    width = 32.0f;
    height = 32.0f;
}

_hudIcon::~_hudIcon()
{
    // dtor
}

void _hudIcon::draw(float screenWidth, float screenHeight)
{
    // Save projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, screenWidth, 0, screenHeight);

    // Save modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // HUD should not use lighting
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f);

    // Convert from top-left style y to OpenGL bottom-left style
    float drawY = screenHeight - pos.y;

    glTranslatef(pos.x, drawY, 0.0f);
    glScalef(width, height, 1.0f);

    myTex->BindTex();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, vboTex);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vboVert);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_LIGHTING);

    // Restore modelview
    glPopMatrix();

    // Restore projection
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

void _hudIcon::initIcon(char* fileName)
{
    myTex->loadTexture(fileName);

    //vertex Buffer
    glGenBuffers(1,&vboVert);
    glBindBuffer(GL_ARRAY_BUFFER,vboVert);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

     //texture Buffer
    glGenBuffers(1,&vboTex);
    glBindBuffer(GL_ARRAY_BUFFER,vboTex);
    glBufferData(GL_ARRAY_BUFFER,sizeof(texCoords),texCoords,GL_STATIC_DRAW);

    //index buffer
    glGenBuffers(1,&eboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
}

void _hudIcon::setPosition(float x, float y, float z)
{
    pos.x = x;
    pos.y = y;
    pos.z = z;
}

