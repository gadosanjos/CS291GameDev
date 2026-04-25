#ifndef _SCENE_H
#define _SCENE_H

#include <_common.h>
#include <_lighting.h>
#include <_skybox.h>
#include <_camera.h>
#include <_sounds.h>
#include "iscene.h"
#include "_hudicon.h"
#include "_ground.h"
#include "_player.h"

class _Scene : public IScene
{
public:
    _Scene();
    virtual ~_Scene() override;

    // IScene interface
    GLint initGL() override;
    void reSize(GLint width, GLint height) override;
    void drawScene() override;
    void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    // scene helpers
    void drawWorld();
    void drawLivesHUD();
    void drawGameOverHUD();
    void resetGame();

    // scene systems
    _lighting* myLight = new _lighting();
    _skyBox* skyBox = new _skyBox();
    _camera* cam = new _camera();
    _sounds* myMusic = new _sounds();

    // world objects
    _ground* worldGround = new _ground();
    _Player* player = new _Player();

    // HUD
    _hudIcon* heartIcon = new _hudIcon();
    _hudIcon* gameOverPanel = new _hudIcon();
    _hudIcon* playAgainPanel = new _hudIcon();
    _hudIcon* exitPanel = new _hudIcon();

    // camera follow settings
    float camFollowDistance = 8.0f;
    float camHeight = 4.0f;
    float camLookHeight = 1.5f;

    // misc scene state
    bool gameOver = false;
    static float deltaT;
    float wWidth, wHeight;
    char* song = "sounds/Conspiracy Theory.mp3";

private:
    std::chrono::steady_clock::time_point lastTime;
};

#endif // _SCENE_H
