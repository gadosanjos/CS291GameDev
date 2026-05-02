#ifndef _TEXLOADER_H
#define _TEXLOADER_H

#include<_common.h>
#include<SOIL.h>

class _texLoader
{
    public:
        _texLoader();
        virtual ~_texLoader();

        int width,height; //keep track of image dementions
        unsigned char* image;
        GLuint texID;

        void loadTexture(char *);
        void BindTex();

    protected:
        bool loadPCXTexture(const char* fileName);
        bool isPCXFile(const char* fileName);

    private:
};

#endif // _TEXLOADER_H
