#ifndef _PARALLAX_H
#define _PARALLAX_H

#include<_common.h>
#include<_texloader.h>


class _parallax
{
    public:
        _parallax();
        virtual ~_parallax();

        void parallaxInit(char*,float,float);
        void parallaxInitMoreControl(char* fileName, float width, float height, float yTop, float zTop, float zBottom);
        void scroll(bool,int,float);
        void drawParallax();
        void updateParallax();

        enum{LEFT,RIGHT,UP,DOWN};
        float xMax = 1.0;
        float yMax = 0.0;
        float xMin = 0.0;
        float yMin = 1.0;

        _texLoader* drawBackground = new _texLoader();

        float vertices[12]; /*= {
            1.0,1.0,0, //top right
            1.0,-1.0,0, //bottom right
            -1.0,-1.0,0, //bottom left
            -1.0,1.0,0, //top left
        };*/ // commented so we do automatically to fill whole screen

        float xHighVert = 1;
        float xLowVert = -1;

        float texcoord[8] ={
            xMax,yMax,
            xMax,yMin,
            xMin,yMin,
            xMin,yMax
        };

        GLuint vboID, texID;
    protected:

    private:
};

#endif // _PARALLAX_H
