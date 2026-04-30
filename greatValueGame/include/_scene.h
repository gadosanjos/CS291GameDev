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
#include "_enemy.h"
#include "_spritesheet.h"
#include "_collision.h"
#include <vector>

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
    void pausedHUD();
    void drawTimerHUD();
    void resetGame();
    void drawText(float x, float y, const char* text, float scale);

    // mob helpers
    void separateEnemies();
    void updateEnemies(float deltaT);
    void drawEnemies();
    void spawnEnemy(float x, float y, float z);
    void spawnPortals(float x, float y, float z);
    void updatePortals(float deltaT);
    void drawPortals();
    void randomizeEnemySpawnPositions(vec3 playerPos, int distanceRange, int minDistance);

    // scene systems
    _lighting* myLight = new _lighting();
    _skyBox* skyBox = new _skyBox();
    _camera* cam = new _camera();
    _sounds* myMusic = new _sounds();
    _collision* myCollision = new _collision();

    // world objects
    _ground* worldGround = new _ground();
    _Player* player = new _Player();
    std::vector<_Enemy*> enemies;
    struct PortalEffect
    {
        _spritesheet* sprite = nullptr;
        float lifetime = 3.0f;
        float age = 0.0f;
    };

    std::vector<PortalEffect> portals;

    // HUD
    _hudIcon* heartIcon = new _hudIcon();
    _hudIcon* gameOverPanel = new _hudIcon();
    _hudIcon* playAgainPanel = new _hudIcon();
    _hudIcon* exitPanel = new _hudIcon();
    _hudIcon* menuPanel = new _hudIcon();
    _hudIcon* helpPanel = new _hudIcon();
    _hudIcon* pausedPanel = new _hudIcon();

    // camera follow settings
    float camFollowDistance = 8.0f;
    float camHeight = 4.0f;
    float camLookHeight = 1.5f;

    // misc scene state
    float enemyRadius = 0.5f;
    float gameTimeRemaining = 300.0f; // 5 minutes in seconds
    bool gameOver = false;
    bool paused = false;
    static float deltaT;
    float wWidth, wHeight;
    char* song = "sounds/Conspiracy Theory.mp3";

private:
    std::chrono::steady_clock::time_point lastTime;
};

#endif // _SCENE_H
