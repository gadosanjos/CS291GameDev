#include "_tutorialscene.h"
#include "_menuscene.h"
#include "scenemanager.h"
#include "_scene.h"

_TutorialScene::_TutorialScene()
{
    //ctor
    lastTime = chrono::steady_clock::now();
}

_TutorialScene::~_TutorialScene()
{
    //dtor
    delete myMusic;
    delete titleIcon;
    delete newGameIcon;
    delete tutorialIcon;
    delete exitIcon;
    delete parallax;
}

GLint _TutorialScene::initGL()
{
    glewInit();

    wWidth  = GetSystemMetrics(SM_CXSCREEN);
    wHeight = GetSystemMetrics(SM_CYSCREEN);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_2D);

    myMusic->iniSounds();
    myMusic->playMusic("sounds/Ancient Civilisations.mp3");

    parallax->parallaxInit("images/tutorialbkg.png", wWidth, wHeight);

    //titleIcon->initIcon("images/menu_title.png");
    newGameIcon->initIcon("images/menu.png");
    tutorialIcon->initIcon("images/tutorial.png");
    exitIcon->initIcon("images/exit.png");

    // Title
    //titleIcon->width = 420.0f;
    //titleIcon->height = 140.0f;

    // Buttons
    buttons[0].icon = newGameIcon;
    buttons[0].anchor = TOP_RIGHT;
    buttons[0].offsetY = -80.0f;
    buttons[0].offsetX = 180.0f;
    buttons[0].width = 260.0f;
    buttons[0].height = 70.0f;
    buttons[0].action = ACTION_NEW_GAME;

    buttons[1].icon = tutorialIcon;
    buttons[1].anchor = TOP_RIGHT;
    buttons[1].offsetY = 10.0f;
    buttons[1].offsetX = 180.0f;
    buttons[1].width = 260.0f;
    buttons[1].height = 70.0f;
    buttons[1].action = ACTION_TUTORIAL;

    buttons[2].icon = exitIcon;
    buttons[2].anchor = TOP_RIGHT;
    buttons[2].offsetY = 100.0f;
    buttons[2].offsetX = 180.0f;
    buttons[2].width = 260.0f;
    buttons[2].height = 70.0f;
    buttons[2].action = ACTION_EXIT;

    return true;

}

void _TutorialScene::reSize(GLint width, GLint height)
{
    wWidth = (float)width;
    wHeight = (float)height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)width / (GLfloat)height, 0.1, 2000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

float _TutorialScene::deltaT = 0;

void _TutorialScene::drawScene()
{
    auto currentTime = chrono::steady_clock::now();

    chrono::duration<float>elapsed = currentTime - lastTime;
    _TutorialScene::deltaT = elapsed.count();
    lastTime =currentTime;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glPushMatrix();
        glTranslatef(0,0,-1.75);
        parallax->drawParallax();
        //parallax->scroll(true, parallax->LEFT, 0.05*deltaT);
        parallax->updateParallax();
    glPopMatrix();

    //titleIcon->pos.x = wWidth * 0.5f;
    //titleIcon->pos.y = 140.0f;
    //titleIcon->draw(wWidth, wHeight);

    for (int i = 0; i < BUTTON_COUNT; i++) {
        drawMenuButton(buttons[i], wWidth, wHeight);
    }
}

void _TutorialScene::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_LBUTTONDOWN:
        {
            float mouseX = (float)LOWORD(lParam);
            float mouseY = (float)HIWORD(lParam);

            for (int i = 0; i < BUTTON_COUNT; i++) {
                if (isMouseOverButton(buttons[i], mouseX, mouseY, wWidth, wHeight)) {
                    handleButtonAction(buttons[i].action, hWnd);
                    break;
                }
            }
            break;
        }

        case WM_KEYDOWN:
            if (wParam == VK_RETURN) {
                handleButtonAction(buttons[0].action, hWnd); // default Enter -> New Game
            }
            break;

        default:
            break;
    }
}

vec2 _TutorialScene::getAnchorPosition(UIAnchor anchor, float screenWidth, float screenHeight)
{
    vec2 p{};

    switch (anchor)
    {
        case TOP_LEFT:    p.x = screenWidth * 0.2f; p.y = screenHeight * 0.15f; break;
        case TOP_CENTER:  p.x = screenWidth * 0.5f; p.y = screenHeight * 0.15f; break;
        case TOP_RIGHT:   p.x = screenWidth * 0.8f; p.y = screenHeight * 0.15f; break;
        case MID_LEFT:    p.x = screenWidth * 0.2f; p.y = screenHeight * 0.5f;  break;
        case MID_CENTER:  p.x = screenWidth * 0.5f; p.y = screenHeight * 0.5f;  break;
        case MID_RIGHT:   p.x = screenWidth * 0.8f; p.y = screenHeight * 0.5f;  break;
        case BOT_LEFT:    p.x = screenWidth * 0.2f; p.y = screenHeight * 0.85f; break;
        case BOT_CENTER:  p.x = screenWidth * 0.5f; p.y = screenHeight * 0.85f; break;
        case BOT_RIGHT:   p.x = screenWidth * 0.8f; p.y = screenHeight * 0.85f; break;
    }

    return p;
}

vec2 _TutorialScene::getButtonPosition(const MenuButton& button, float screenWidth, float screenHeight)
{
    vec2 base = getAnchorPosition(button.anchor, screenWidth, screenHeight);

    vec2 finalPos{};
    finalPos.x = base.x + button.offsetX;
    finalPos.y = base.y + button.offsetY;

    return finalPos;
}

void _TutorialScene::drawMenuButton(MenuButton& button, float screenWidth, float screenHeight)
{
    vec2 pos = getButtonPosition(button, screenWidth, screenHeight);

    button.icon->pos.x = pos.x;
    button.icon->pos.y = pos.y;
    button.icon->width = button.width;
    button.icon->height = button.height;

    button.icon->draw(screenWidth, screenHeight);
}

bool _TutorialScene::isMouseOverButton(const MenuButton& button, float mouseX, float mouseY, float screenWidth, float screenHeight)
{
    vec2 pos = getButtonPosition(button, screenWidth, screenHeight);

    float left   = pos.x - button.width * 0.5f;
    float right  = pos.x + button.width * 0.5f;
    float top    = pos.y - button.height * 0.5f;
    float bottom = pos.y + button.height * 0.5f;

    return mouseX >= left && mouseX <= right &&
           mouseY >= top  && mouseY <= bottom;
}

void _TutorialScene::handleButtonAction(MenuAction action, HWND hWnd)
{
switch (action)
    {
        case ACTION_NEW_GAME:
            if (manager) {
                manager->changeScene(new _MenuScene());
            }
            break;

        case ACTION_TUTORIAL:
            // later: manager->changeScene(new _TutorialScene());
            break;

        case ACTION_EXIT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;

        default:
            break;
    }
}
