#ifndef _PAUSEDSCENE_H
#define _PAUSEDSCENE_H

#include <_common.h>
#include <_inputs.h>
#include <_texloader.h>
#include <_parallax.h>
#include <_sounds.h>
#include "iscene.h"
#include "_hudicon.h"

class _pausedscene : public IScene
{
    public:
        _pausedscene();
        virtual ~_pausedscene();

        GLint initGL() override;    // Initializing my game objects and opengl
        void reSize(GLint,GLint) override; //window resizing
        void drawScene() override;  //rendering the scene
        void WndProc(
              HWND	hWnd,			// Handle For This Window
			  UINT	uMsg,			// Message For This Window
			  WPARAM wParam,		// Additional Message Information
			  LPARAM lParam) override;		// Additional Message Information

        bool isMouseOverButton(_hudIcon* button, float mouseX, float mouseY, float screenWidth, float screenHeight);

        _inputs *keyMS  = new _inputs();
        _parallax *parallax = new _parallax();
        _sounds *myMusic = new _sounds();

        float wWidth, wHeight;
        static float deltaT;
        _hudIcon* menuIcon = new _hudIcon();

    protected:
    private:
        std::chrono::steady_clock::time_point lastTime;
};

#endif // _PAUSEDSCENE_H

