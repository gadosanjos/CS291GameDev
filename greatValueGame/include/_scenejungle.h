#ifndef _SCENEJUNGLE_H
#define _SCENEJUNGLE_H

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
#include "_bullets.h"

class _SceneJungle : public IScene
{
public:
    _SceneJungle();
    virtual ~_SceneJungle() override;

    // IScene interface
    GLint initGL() override;
    void reSize(GLint width, GLint height) override;
    void drawScene() override;
    void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    // scene helpers
    void drawWorld();
    void drawLivesHUD();
    void drawGameOverHUD();
    void drawGameWinHUD();
    void pausedHUD();
    void drawTimerHUD();
    void resetGame();
    void drawText(float x, float y, const char* text, float scale);
    void checkEnemyPlayerDamage(float deltaT);

    // mob helpers
    void separateEnemies();
    void updateEnemies(float deltaT);
    void drawEnemies();
    void spawnEnemy(float x, float y, float z, int selection);
    void spawnPortals(float x, float y, float z, int portalCollor);
    void updatePortals(float deltaT);
    void drawPortals();
    void randomizeEnemySpawnPositions(vec3 playerPos, int distanceRange, int minDistance, float timeRemaining);
    void clearEnemies();
    void clearPortals();
    void initMagicBullets();
    _Enemy* findNearestEnemyInRange(float range);
    void updateAutoMagic(float deltaT);
    void updateMagicBullets(float deltaT);
    void drawMagicBullets();
    void removeDeadEnemies();

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
    static const int MAX_MAGIC_BULLETS = 20;
    _bullets magicBullets[MAX_MAGIC_BULLETS];

    int nextBulletIndex = 0;

    // HUD
    _hudIcon* heartIcon = new _hudIcon();
    _hudIcon* gameOverPanel = new _hudIcon();
    _hudIcon* gameWinPanel = new _hudIcon();
    _hudIcon* playAgainPanel = new _hudIcon();
    _hudIcon* playNextPanel = new _hudIcon();
    _hudIcon* exitPanel = new _hudIcon();
    _hudIcon* menuPanel = new _hudIcon();
    _hudIcon* helpPanel = new _hudIcon();
    _hudIcon* pausedPanel = new _hudIcon();

    // camera follow settings
    float camFollowDistance = 8.0f;
    float camHeight = 4.0f;
    float camLookHeight = 1.5f;

    // misc scene state
    float playerHitRadius = 1.0f;
    float enemyAttackRadius = 1.2f;
    float enemyDamageCooldown = 0.0f;
    float enemyDamageCooldownTime = 1.0f;
    int enemyContactDamage = 1;
    float deathAnimationPeriod = 3.0f;
    float enemyRadius = 0.5f;
    float gameTimeRemaining = 10.0f; // 5 minutes in seconds
    bool gameOver = false;
    bool gameWin = false;
    bool paused = false;
    static float deltaT;
    float wWidth, wHeight;
    char* song = "sounds/Conspiracy Theory.mp3";

private:
    std::chrono::steady_clock::time_point lastTime;
    enum {dragonknight, knight, zealot, werewolf1, werewolf2, wolf} mobSelection;
};

#endif // _SceneJungle_H
