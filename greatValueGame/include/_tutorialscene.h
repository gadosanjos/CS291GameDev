#ifndef _TUTORIALSCENE_H
#define _TUTORIALSCENE_H

#include "_common.h"
#include "_inputs.h"
#include "_hudicon.h"
#include "_sounds.h"
#include "iscene.h"
#include "_parallax.h"

class _TutorialScene : public IScene
{
public:
    _TutorialScene();
    virtual ~_TutorialScene();

    GLint initGL() override;
    void reSize(GLint, GLint) override;
    void drawScene() override;
    void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    enum UIAnchor
    {
        TOP_LEFT,
        TOP_CENTER,
        TOP_RIGHT,
        MID_LEFT,
        MID_CENTER,
        MID_RIGHT,
        BOT_LEFT,
        BOT_CENTER,
        BOT_RIGHT
    };

    enum MenuAction
    {
        ACTION_NEW_GAME,
        ACTION_TUTORIAL,
        ACTION_EXIT,
        ACTION_BACK,
        ACTION_NONE
    };

    struct MenuButton
    {
        _hudIcon* icon = nullptr;

        UIAnchor anchor = MID_CENTER;
        float offsetX = 0.0f;
        float offsetY = 0.0f;

        float width = 200.0f;
        float height = 60.0f;

        MenuAction action = ACTION_NONE;
    };

    vec2 getAnchorPosition(UIAnchor anchor, float screenWidth, float screenHeight);
    vec2 getButtonPosition(const MenuButton& button, float screenWidth, float screenHeight);
    void drawMenuButton(MenuButton& button, float screenWidth, float screenHeight);
    bool isMouseOverButton(const MenuButton& button, float mouseX, float mouseY, float screenWidth, float screenHeight);
    void handleButtonAction(MenuAction action, HWND hWnd);

    float wWidth, wHeight;
    static float deltaT;

    _hudIcon* titleIcon = new _hudIcon();
    _hudIcon* newGameIcon = new _hudIcon();
    _hudIcon* tutorialIcon = new _hudIcon();
    _hudIcon* exitIcon = new _hudIcon();
    _sounds* myMusic = new _sounds();
    _parallax *parallax = new _parallax();

    static const int BUTTON_COUNT = 3;
    MenuButton buttons[BUTTON_COUNT];

private:
    std::chrono::steady_clock::time_point lastTime;
};

#endif // _TUTORIALSCENE_H
