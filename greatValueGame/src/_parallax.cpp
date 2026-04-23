#include "_parallax.h"

_parallax::_parallax()
{
    //ctor
    xMax = 1.0;
    yMax = 0.0;
    xMin = 0.0;
    yMin = 1.0;
}

_parallax::~_parallax()
{
    //dtor
}
void _parallax::parallaxInit(char* fileName, float width, float height)
{
    drawBackground->loadTexture(fileName);

    xHighVert = width/height;
    xLowVert = -width/height;

    // even though this vertices is local it lives on on the buffer till the end of the program
    float vertices[12] = {
            xHighVert,1.0,0, //top right
            xHighVert,-1.0,0, //bottom right
            xLowVert,-1.0,0, //bottom left
            xLowVert,1.0,0, //top left
    };

    //vertex Buffer
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    //texture Buffer
    glGenBuffers(1, &texID);
    glBindBuffer(GL_ARRAY_BUFFER, texID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_DYNAMIC_DRAW);
}

void _parallax::parallaxInitMoreControl(char* fileName, float width, float height, float yTop, float zTop, float zBottom)
{
    drawBackground->loadTexture(fileName);
    xHighVert = width/height;
    xLowVert = -width/height;

    // even though this vertices is local it lives on on the buffer till the end of the program
    float vertices[12] = {
            xHighVert, yTop, zTop, //top right
            xHighVert, -1.0, zBottom, //bottom right
            xLowVert,  -1.0, zBottom, //bottom left
            xLowVert,  yTop,zTop, //top left
    };

    //vertex Buffer
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    //texture Buffer
    glGenBuffers(1, &texID);
    glBindBuffer(GL_ARRAY_BUFFER, texID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_DYNAMIC_DRAW);
}

void _parallax::scroll(bool automaticMove, int directions, float speed)
{
    if(automaticMove){
        switch(directions)
        {
            case LEFT:
                xMin += speed;
                xMax += speed;
                break;
            case RIGHT:
                xMin -= speed;
                xMax -= speed;
                break;
            case UP:
                yMin += speed;
                yMax += speed;
                break;
            case DOWN:
                yMin -= speed;
                yMax -= speed;
                break;
            default:
                break;
        }
    }
}

void _parallax::drawParallax()
{
    glPushMatrix();

        glScalef(1,1,1);

        glEnableClientState(GL_VERTEX_ARRAY);
        //glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, texID);
        glTexCoordPointer(2,GL_FLOAT,0,(void*)0);

        //glBindBuffer(GL_ARRAY_BUFFER, vboNorm);
        //glNormalPointer(GL_FLOAT,0,0);

        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glVertexPointer(3,GL_FLOAT, 0,0);

        drawBackground->BindTex();
        //glDrawArrays(GL_TRIANGLES,0, 12);
        glDrawArrays(GL_QUADS,0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        //glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        glBindVertexArray(0);


    glPopMatrix();
}

void _parallax::updateParallax()
{
    float texcoord[8] ={
        xMax,yMax,
        xMax,yMin,
        xMin,yMin,
        xMin,yMax
    };
    glBindBuffer(GL_ARRAY_BUFFER, texID);
    glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(texcoord), texcoord);
}
