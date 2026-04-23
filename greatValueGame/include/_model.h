#ifndef _MODEL_H
#define _MODEL_H

#include <_common.h>
#include <_texloader.h>

class _model
{
    public:
        _model();
        virtual ~_model();

        vec3 pos; // for translation
        vec3 scale; //resizing the model
        vec3 rot; // rotate the model

        _texLoader *myTex = new _texLoader();

        void drawModel(); // renderer
        void initModel(char *); //initialization

    protected:

    private:
};

#endif // _MODEL_H
