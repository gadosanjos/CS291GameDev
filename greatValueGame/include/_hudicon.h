#ifndef _HUDICON_H
#define _HUDICON_H

#include "_common.h"
#include "_texloader.h"

class _hudIcon
{
    public:
        _hudIcon();
        virtual ~_hudIcon();

        vec3 pos;
        float width;
        float height;

        void initIcon(char* fileName);
        void setPosition(float x,float y,float z);
        void draw(float screenWidth, float screenHeight);

        _texLoader* myTex = new _texLoader();

        GLuint vboVert, vboTex, eboID;

        // 2D quad centered at origin
        GLfloat vertices[12] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
        };

        GLfloat texCoords[8] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f
        };

        GLuint indices[6] = {
            0, 1, 2,
            0, 2, 3
        };



    protected:

    private:
};

#endif // _HUDICON_H
