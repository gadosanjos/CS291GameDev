#ifndef _INPUTS_H
#define _INPUTS_H

#include<_common.h>
#include<_model.h>
#include<_modelvbo.h>
#include<_skybox.h>
#include<_camera.h>
#include<_spritesheet.h>
#include<_ModelLoaderMD2.h>

class _inputs
{
    public:
        _inputs();
        virtual ~_inputs();

        void keyPressed(_model*);
        void keyPressed(_modelVBO*);
        void keyPressed(_skyBox*);
        void keyPressed(_camera*);
        void keyPressed(_spritesheet*);
        void keyUp();

        bool isTranslate;
        bool isRotate;

        double prev_mX;
        double prev_mY;

        void mouseEvenDown(double,double);
        void mouseUp();
        void mouseMove(_model*,double,double);

        void mouseWheel(_model*,double);

        void mouseMove(_modelVBO*,double,double);
        void mouseMove(_skyBox*,double,double);
        void mouseWheel(_modelVBO*,double);
        void mouseWheel(_camera*,double);

        void keyPressed(_ModelLoaderMD2*,_ModelLoaderMD2* );

        WPARAM wParam;

    protected:

    private:
};

#endif // _INPUTS_H
