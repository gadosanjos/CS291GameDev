#ifndef _LANDINGSCENE_H
#define _LANDINGSCENE_H

#include <_common.h>
#include <_inputs.h>
#include <_texloader.h>
#include <_parallax.h>
#include <_sounds.h>
#include "iscene.h"
#include "_hudicon.h"

class _LandingScene : public IScene
{
    public:
        _LandingScene();
        virtual ~_LandingScene() override;

        GLint initGL() override;    // Initializing my game objects and opengl
        void reSize(GLint,GLint) override; //window resizing
        void drawScene() override;  //rendering the scene
        void WndProc(
              HWND	hWnd,			// Handle For This Window
			  UINT	uMsg,			// Message For This Window
			  WPARAM wParam,		// Additional Message Information
			  LPARAM lParam) override;		// Additional Message Information


        _inputs *keyMS  = new _inputs();
        _parallax *parallax = new _parallax();
        _sounds *myMusic = new _sounds();

        float wWidth, wHeight;
        static float deltaT;

    protected:
    private:
        std::chrono::steady_clock::time_point lastTime;
};

#endif // _LANDINGSCENE_H
