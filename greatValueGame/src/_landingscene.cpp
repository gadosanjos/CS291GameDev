#include "_landingscene.h"
#include "scenemanager.h"
#include "_scene.h"
#include "_menuscene.h"

_LandingScene::_LandingScene()
{
    //ctor
    lastTime = chrono::steady_clock::now();
}

_LandingScene::~_LandingScene()
{
    //dtor
    delete parallax;
    delete myMusic;
    delete keyMS;
}
GLint _LandingScene::initGL()
{
    glewInit();

    wWidth  = GetSystemMetrics(SM_CXSCREEN);
    wHeight = GetSystemMetrics(SM_CYSCREEN);

    glClearColor(0.0,0.0,0.0,1.0); // setting background color
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);


    myMusic->iniSounds();
    myMusic->playMusic("sounds/Horizons.mp3");

    parallax->parallaxInit("images/parallax.jpg", wWidth, wHeight);

    return true;
}

void _LandingScene::reSize(GLint width, GLint height)
{
    GLfloat aspectRatio = (GLfloat)width/(GLfloat)height;
    glViewport(0,0,width,height); //adjusting the viewport
    glMatrixMode(GL_PROJECTION);  //for perspective projection
    glLoadIdentity();

    gluPerspective(60,aspectRatio,0.1,2000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}
float _LandingScene::deltaT = 0;

void _LandingScene::drawScene()
{
    auto currentTime = chrono::steady_clock::now();

    chrono::duration<float>elapsed = currentTime - lastTime;
    _LandingScene::deltaT = elapsed.count();
    lastTime =currentTime;

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glPushMatrix();
        glTranslatef(0,0,-1.75);
        parallax->drawParallax();
        parallax->scroll(true, parallax->LEFT, 0.05*deltaT);
        parallax->updateParallax();
    glPopMatrix();
}

void _LandingScene::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_KEYDOWN:
            keyMS->wParam = wParam;
            if (wParam == VK_RETURN) {
                manager->changeScene(new _MenuScene());
            }
            break;
        case WM_KEYUP:
            break;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            keyMS->wParam = wParam;
            keyMS->mouseEvenDown(LOWORD(lParam), HIWORD(lParam));
            manager->changeScene(new _MenuScene());
            break;
        case WM_MBUTTONDOWN:
            break;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            keyMS->mouseUp();
            break;
        case WM_MOUSEMOVE:
            break;
        case WM_MOUSEWHEEL:
            break;
        default:
            break;
    }
}
