#ifndef _GROUND_H
#define _GROUND_H

#include "_common.h"
#include "_texloader.h"
class _ground
{
    public:
        _ground();
        virtual ~_ground();
        vec3 rot;
        vec3 pos;
        vec3 scale;

        void drawModel();
        void modelInit(char *);
        void setPosition(float x,float y,float z);
        void moveZ(float dz);


        _texLoader *myTex = new _texLoader();

        GLuint vboVert, vboNorm,vboTex, eboID;

         GLfloat vertices[12] = {
            -0.5f, 0.0f, -0.5f,
             0.5f, 0.0f, -0.5f,
             0.5f, 0.0f,  0.5f,
            -0.5f, 0.0f,  0.5f
        };

        GLfloat normals[12] = {
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f
        };

        GLfloat texCoords[8] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        GLuint indices[6] = {
            0, 1, 2,
            0, 2, 3
        };
    protected:

    private:
};

#endif // _GROUND_H
