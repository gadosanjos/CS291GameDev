#include "_pausedscene.h"
#include "_tutorialscene.h"
#include "_menuscene.h"
#include "_scene.h"
#include "scenemanager.h"

_pausedscene::_pausedscene()
{
    lastTime = chrono::steady_clock::now();
    //ctor
}

_pausedscene::~_pausedscene()
{
    //dtor
}

GLint _pausedscene::initGL()
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

    //menuIcon->initIcon("images/menubutton.png");
    //menuIcon->width = 500;
    //menuIcon->height = 500;
    //menuIcon->pos.x = 1500;
    //menuIcon->pos.y = 500;

    //myMusic->iniSounds();
    //myMusic->playMusic("sounds/Horizons.mp3");

    parallax->parallaxInit("images/paused.png", wWidth, wHeight);

    return true;
}

float _pausedscene::deltaT = 0;

void _pausedscene::reSize(GLint width, GLint height)
{
    GLfloat aspectRatio = (GLfloat)width/(GLfloat)height;
    glViewport(0,0,width,height); //adjusting the viewport
    glMatrixMode(GL_PROJECTION);  //for perspective projection
    glLoadIdentity();

    gluPerspective(60,aspectRatio,0.1,2000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void _pausedscene::drawScene()
{
    auto currentTime = chrono::steady_clock::now();

    chrono::duration<float>elapsed = currentTime - lastTime;
    _pausedscene::deltaT = elapsed.count();
    lastTime =currentTime;

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    menuIcon->draw(wWidth, wHeight);

    glPushMatrix();
        glTranslatef(0,0,-1.75);
        parallax->drawParallax();
    glPopMatrix();
}

void _pausedscene::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_KEYDOWN:
            keyMS->wParam = wParam;
            if (wParam == 'H' || wParam == 'h'){
                manager->changeScene(new _TutorialScene());
            }
            if (wParam == 'G' || wParam == 'g'){
                manager->changeScene(new _Scene());
            }
            if (wParam == VK_RETURN) {
                manager->changeScene(new _MenuScene());
            }
            break;
        case WM_KEYUP:
            break;
        case WM_LBUTTONDOWN:
            {
                float mouseX = (float)LOWORD(lParam);
                float mouseY = (float)HIWORD(lParam);

                break;
            }
        case WM_RBUTTONDOWN:
            keyMS->wParam = wParam;
            keyMS->mouseEvenDown(LOWORD(lParam), HIWORD(lParam));
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
