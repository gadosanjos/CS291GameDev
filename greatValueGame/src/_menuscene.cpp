#include "_menuscene.h"
#include "scenemanager.h"
#include "_scene.h"
#include "_tutorialscene.h"

_MenuScene::_MenuScene()
{
    lastTime = chrono::steady_clock::now();
}

_MenuScene::~_MenuScene()
{
    delete myMusic;
    delete titleIcon;
    delete newGameIcon;
    delete tutorialIcon;
    delete exitIcon;
    delete parallax;
}

GLint _MenuScene::initGL()
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
    myMusic->playMusic("sounds/Duty Calls.mp3");

    parallax->parallaxInit("images/menu/bkg.png", wWidth, wHeight);

    titleIcon->initIcon("images/menu/menu_title.png");
    newGameIcon->initIcon("images/menu/newgame.png");
    tutorialIcon->initIcon("images/menu/tutorial.png");
    exitIcon->initIcon("images/menu/exit.png");

    // Title
    titleIcon->width = 1240.0f;
    titleIcon->height = 600.0f;

    // Buttons
    buttons[0].icon = newGameIcon;
    buttons[0].anchor = MID_CENTER;
    buttons[0].offsetY = 50.0f;
    buttons[0].width = 520.0f;
    buttons[0].height = 320.0f;
    buttons[0].action = ACTION_NEW_GAME;

    buttons[1].icon = tutorialIcon;
    buttons[1].anchor = MID_CENTER;
    buttons[1].offsetY = 250.0f;
    buttons[1].width = 520.0f;
    buttons[1].height = 320.0f;
    buttons[1].action = ACTION_TUTORIAL;

    buttons[2].icon = exitIcon;
    buttons[2].anchor = MID_CENTER;
    buttons[2].offsetY = 450.0f;
    buttons[2].width = 520.0f;
    buttons[2].height = 320.0f;
    buttons[2].action = ACTION_EXIT;

    return true;
}

void _MenuScene::reSize(GLint width, GLint height)
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

vec2 _MenuScene::getAnchorPosition(UIAnchor anchor, float screenWidth, float screenHeight)
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

vec2 _MenuScene::getButtonPosition(const MenuButton& button, float screenWidth, float screenHeight)
{
    vec2 base = getAnchorPosition(button.anchor, screenWidth, screenHeight);

    vec2 finalPos{};
    finalPos.x = base.x + button.offsetX;
    finalPos.y = base.y + button.offsetY;

    return finalPos;
}

void _MenuScene::drawMenuButton(MenuButton& button, float screenWidth, float screenHeight)
{
    vec2 pos = getButtonPosition(button, screenWidth, screenHeight);

    button.icon->pos.x = pos.x;
    button.icon->pos.y = pos.y;
    button.icon->width = button.width;
    button.icon->height = button.height;

    button.icon->draw(screenWidth, screenHeight);
}

bool _MenuScene::isMouseOverButton(const MenuButton& button, float mouseX, float mouseY, float screenWidth, float screenHeight)
{
    vec2 pos = getButtonPosition(button, screenWidth, screenHeight);

    float left   = pos.x - button.width * 0.5f;
    float right  = pos.x + button.width * 0.5f;
    float top    = pos.y - button.height * 0.5f;
    float bottom = pos.y + button.height * 0.5f;

    return mouseX >= left && mouseX <= right &&
           mouseY >= top  && mouseY <= bottom;
}

void _MenuScene::handleButtonAction(MenuAction action, HWND hWnd)
{
    switch (action)
    {
        case ACTION_NEW_GAME:
            if (manager) {
                manager->changeScene(new _Scene());
            }
            break;

        case ACTION_TUTORIAL:
            manager->changeScene(new _TutorialScene());
            break;

        case ACTION_EXIT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;

        default:
            break;
    }
}

float _MenuScene::deltaT = 0;

void _MenuScene::drawScene()
{
    auto currentTime = chrono::steady_clock::now();

    chrono::duration<float>elapsed = currentTime - lastTime;
    _MenuScene::deltaT = elapsed.count();
    lastTime =currentTime;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glPushMatrix();
        glTranslatef(0,0,-1.75);
        parallax->drawParallax();
        parallax->scroll(true, parallax->LEFT, 0.05*deltaT);
        parallax->updateParallax();
    glPopMatrix();

    titleIcon->pos.x = wWidth * 0.5f;
    titleIcon->pos.y = 300.0f;
    titleIcon->draw(wWidth, wHeight);

    for (int i = 0; i < BUTTON_COUNT; i++) {
        drawMenuButton(buttons[i], wWidth, wHeight);
    }
}

void _MenuScene::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
