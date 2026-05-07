#include "_SceneJungle.h"
#include "scenemanager.h"
#include "_menuscene.h"
#include "_tutorialscene.h"
#include "_pausedscene.h"
#include <cmath>
#include "_scenedungeon.h"

// ======================================================
// Construction / destruction
// ======================================================

_SceneJungle::_SceneJungle()
{
    lastTime = chrono::steady_clock::now();
}

_SceneJungle::~_SceneJungle()
{
    delete myLight;
    delete skyBox;
    delete cam;
    delete myMusic;
    delete worldGround;
    delete player;
    delete myCollision;

    clearEnemies();
    clearPortals();
}

// ======================================================
// Core scene lifecycle
// ======================================================

GLint _SceneJungle::initGL()
{
    glewInit();

    wWidth  = GetSystemMetrics(SM_CXSCREEN);
    wHeight = GetSystemMetrics(SM_CYSCREEN);

    glClearColor(0.35f, 0.55f, 0.75f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);

    myLight->setLight(GL_LIGHT0);

    myMusic->iniSounds();
    Sleep(250);
    myMusic->playMusic(song);

    // skybox
    skyBox->boxInit();
    skyBox->myTex[0].loadTexture("images/ProjectSkyboxes/Field_Box_Back.png");
    skyBox->myTex[1].loadTexture("images/ProjectSkyboxes/Field_Box_Front.png");
    skyBox->myTex[2].loadTexture("images/ProjectSkyboxes/Field_Box_Top.png");
    skyBox->myTex[3].loadTexture("images/ProjectSkyboxes/Field_Box_Bottom.png");
    skyBox->myTex[4].loadTexture("images/ProjectSkyboxes/Field_Box_Left.png");
    skyBox->myTex[5].loadTexture("images/ProjectSkyboxes/Field_Box_Right.png");
    // HUD
    heartIcon->initIcon("images/heart.png");
    heartIcon->width = 60.0f;
    heartIcon->height = 60.0f;
    gameOverPanel->initIcon("images/menu/gameover.png");
    gameOverPanel->width = 1000.0f;
    gameOverPanel->height = 520.0f;
    pausedPanel->initIcon("images/menu/paused.png");
    pausedPanel->width = 1000.0f;
    pausedPanel->height = 520.0f;
    playAgainPanel->initIcon("images/menu/newgame.png");
    playAgainPanel->width = 500.0f;
    playAgainPanel->height = 300.0f;
    helpPanel->initIcon("images/menu/tutorial.png");
    helpPanel->width = 500.0f;
    helpPanel->height = 300.0f;
    menuPanel->initIcon("images/menu/menu.png");
    menuPanel->width = 500.0f;
    menuPanel->height = 300.0f;
    exitPanel->initIcon("images/menu/exit.png");
    exitPanel->width = 500.0f;
    exitPanel->height = 300.0f;
    gameWinPanel->initIcon("images/transition/lvl2complete.png");
    gameWinPanel->width = 1000.0f;
    gameWinPanel->height = 520.0f;
    playNextPanel->initIcon("images/transition/nextlevel.png");
    playNextPanel->width = 500.0f;
    playNextPanel->height = 300.0f;

    // player model
    player->init("models/doomguy/tris.md2", "models/doomguy/camo.pcx", "models/doomguy/w_shotgun.md2", "models/doomguy/w_shotgun.pcx");
    player->pos.x = 0.0f;
    player->pos.y = 0.0f;
    player->pos.z = 0.0f;
    player->yaw = 0.0f;

    // One enemy initialization so i dont forget
    spawnPortals(0.0f, 0.0f, -20.0f, 1);
    spawnEnemy(0.0f, 0.0f, -20.0f, dragonknight);
    spawnEnemy(-10.0f, 0.0f, -20.0f, knight);
    spawnEnemy(10.0f, 0.0f, -20.0f, zealot);
    //spawnEnemy(-20.0f, 0.0f, -20.0f, werewolf1);
    //spawnEnemy(20.0f, 0.0f, -20.0f, werewolf2);
    //spawnEnemy(-30.0f, 0.0f, -20.0f, wolf);

    // world ground
    worldGround->modelInit("images/fieldtexture.png");
    worldGround->scale.x = 1000.0f;
    worldGround->scale.z = 1000.0f;
    worldGround->pos.x = 0.0f;
    worldGround->pos.y = -2.0f;
    worldGround->pos.z = 0.0f;

    // magic bullets / auto attack
    initMagicBullets();
    paused = true;


    cam->followTarget(player->pos, player->yaw, camFollowDistance, camHeight, camLookHeight);
    //player->playerLives = 10000; // game over debug
    return true;
}

void _SceneJungle::reSize(GLint width, GLint height)
{
    wWidth = (float)width;
    wHeight = (float)height;

    GLfloat aspectRatio = (GLfloat)width / (GLfloat)height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60, aspectRatio, 0.1, 2000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

float _SceneJungle::deltaT = 0.0f;

void _SceneJungle::drawScene()
{
    // --------------------------------------
    // 1. update frame timing
    // --------------------------------------

    auto currentTime = chrono::steady_clock::now();
    chrono::duration<float> elapsed = currentTime - lastTime;
    _SceneJungle::deltaT = elapsed.count();
    lastTime = currentTime;

    // update game timer
    if (!gameOver) {
        if(paused){
            //return;
        } else {
            gameTimeRemaining -= deltaT;
        }

        if (gameTimeRemaining <= 0.0f) {
            gameTimeRemaining = 0.0f;
            gameWin = true;
        }
    }

    // --------------------------------------
    // 2. clear frame and set camera
    // --------------------------------------

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    cam->setUpCamera();

    // --------------------------------------
    // 3. draw background / sky
    // --------------------------------------

    glPushMatrix();
        skyBox->drawBox();
    glPopMatrix();

    drawWorld();

    // --------------------------------------
    // 4. update
    // --------------------------------------

    if(player->playerLives <= 0){
        player->setDeath(true);
        gameOver = true;
    }
    if(paused){
        pausedHUD();
        return;
    }
    if(gameOver){
        deathAnimationPeriod -= deltaT;
        if(deathAnimationPeriod <= 0){
            deathAnimationPeriod = 0;
            drawGameOverHUD();
            return;
        }
    }
    if(gameWin){
        drawGameWinHUD();
        return;
    }
    if(enemies.size() < 10){
        randomizeEnemySpawnPositions(player->pos, 10, 25, gameTimeRemaining);
    }

    player->update(deltaT);
    updatePortals(deltaT);
    updateEnemies(deltaT);
    separateEnemies();

    checkEnemyPlayerDamage(deltaT);

    updateAutoMagic(deltaT);
    updateMagicBullets(deltaT);
    removeDeadEnemies();

    cam->followTarget(player->pos, player->yaw, camFollowDistance, camHeight, camLookHeight);

    // --------------------------------------
    // 5. draw gameplay entities
    // --------------------------------------

    player->draw();
    drawPortals();
    drawEnemies();
    drawMagicBullets();
}

// ======================================================
// Scene helpers
// ======================================================

void _SceneJungle::drawWorld(){
    glPushMatrix();
        worldGround->drawModel();
        drawLivesHUD();
        drawTimerHUD();
    glPopMatrix();
}

void _SceneJungle::checkEnemyPlayerDamage(float deltaT){
    // If player is already dead or game is paused, do not apply damage.
    if (gameOver || paused) {
        return;
    }

    // Cooldown prevents damage every single frame.
    if (enemyDamageCooldown > 0.0f) {
        enemyDamageCooldown -= deltaT;
    }

    // If still cooling down, no new damage this frame.
    if (enemyDamageCooldown > 0.0f) {
        return;
    }

    for (size_t i = 0; i < enemies.size(); i++) {
        _Enemy* enemy = enemies[i];

        if (!enemy->active || enemy->dead) {
            continue;
        }

        bool touchingPlayer = myCollision->checkRadiusHit(
            player->pos,
            playerHitRadius,
            enemy->pos,
            enemyAttackRadius
        );

        if (touchingPlayer) {
            player->playerLives -= enemyContactDamage;

            if (player->playerLives < 0) {
                player->playerLives = 0;
            }

            enemyDamageCooldown = enemyDamageCooldownTime;

            cout << "Player hit by enemy! Lives: "
                 << player->playerLives << endl;

            break;
        }
    }
}

// ======================================================
// Mob helpers
// ======================================================
void _SceneJungle::randomizeEnemySpawnPositions(vec3 playerPos, int distanceRange, int minDistance, float timeRemaining)
{
    float angle = ((float)rand() / RAND_MAX) * 2.0f * (float)PI;
    float radius = minDistance + ((float)rand() / RAND_MAX) * distanceRange;

    float spawnX = playerPos.x + cosf(angle) * radius;
    float spawnZ = playerPos.z + sinf(angle) * radius;

    spawnPortals(spawnX, 0.0f, spawnZ-1, 0);
    spawnEnemy(spawnX, 0.0f, spawnZ, 1);

    if(timeRemaining <= 60.0f){
        spawnPortals(spawnX/2, 0.0f, spawnZ-1, 1);
        spawnEnemy(spawnX/2, 0.0f, spawnZ, dragonknight);
    } else if (timeRemaining <= 90.f){
        spawnPortals(spawnX/2, 0.0f, spawnZ-1, 1);
        spawnEnemy(spawnX/2, 0.0f, spawnZ, zealot);
    } else if (timeRemaining <= 110.f){
        spawnPortals(spawnX/2, 0.0f, spawnZ-1, 1);
        spawnEnemy(spawnX/2, 0.0f, spawnZ, werewolf1);
    }

}

void _SceneJungle::spawnPortals(float x, float y, float z, int portalCollor)
{
    PortalEffect p;

    p.sprite = new _spritesheet();
    if(portalCollor == 0){
        p.sprite->spriteInit("images/blueportal.png", 1, 1);
    } else {
        p.sprite->spriteInit("images/orangeportal.png", 1, 1);
    }
    p.sprite->pos.x = x;
    p.sprite->pos.y = y;
    p.sprite->pos.z = z;

    p.sprite->scale.x = 2.0f;
    p.sprite->scale.y = 2.0f;
    p.sprite->scale.z = 1.0f;

    p.lifetime = 3.0f; // portal stays for 3 seconds
    p.age = 0.0f;

    portals.push_back(p);
}

void _SceneJungle::spawnEnemy(float x, float y, float z, int mobSelection)
{
    _Enemy* e = new _Enemy();
    switch(mobSelection){
        case dragonknight:
            e->init("models/dragonknight/tris.md2", "models/dragonknight/dragon_armour.pcx", "models/dragonknight/weapon.md2", "models/dragonknight/knight.pcx");
            break;
        case knight:
            e->init("models/dragonknight/tris.md2", "models/dragonknight/dragon_green.pcx", "models/dragonknight/weapon.md2", "models/dragonknight/knight.pcx");
            break;
        case zealot:
            e->init("models/dragonknight/tris.md2", "models/dragonknight/dragon_yellow.pcx", "models/dragonknight/weapon.md2", "models/dragonknight/knight.pcx");
            break;
        case werewolf1:
            e->init("models/wolves/werewolf1/tris.md2", "models/wolves/werewolf1/rage.pcx", "models/wolves/werewolf1/weapon.md2", "models/wolves/werewolf1/rage.pcx");
            break;
        case werewolf2:
            e->init("models/wolves/werewolf2/tris.md2", "models/wolves/werewolf2/werewolf.pcx", "models/wolves/werewolf2/weapon.md2", "models/wolves/werewolf2/werewolf.pcx");
            break;
        case wolf:
            e->init("models/wolves/wolf/tris.md2", "models/wolves/wolf/Anya.pcx", "models/wolves/wolf/w_bfg.md2", "models/wolves/wolf/W_bfg.pcx");
            break;
        default:
            break;

    }
    e->pos.x = x;
    e->pos.y = y;
    e->pos.z = z;
    enemies.push_back(e);
}

void _SceneJungle::updatePortals(float deltaT)
{
    for (int i = (int)portals.size() - 1; i >= 0; i--) {
        portals[i].age += deltaT;

        if (portals[i].age >= portals[i].lifetime) {
            delete portals[i].sprite;
            portals.erase(portals.begin() + i);
        }
    }
}

void _SceneJungle::updateEnemies(float deltaT)
{
    for (size_t i = 0; i < enemies.size(); i++) {
        enemies[i]->update(deltaT, player->pos);
    }
}

void _SceneJungle::drawPortals()
{
    for (size_t i = 0; i < portals.size(); i++) {
        if (portals[i].sprite) {
            portals[i].sprite->drawSprite();
        }
    }
}

void _SceneJungle::drawEnemies()
{
    for (size_t i = 0; i < enemies.size(); i++) {
        enemies[i]->draw();
    }
}

void _SceneJungle::separateEnemies()
{
    for (size_t i = 0; i < enemies.size(); i++) {
        for (size_t j = i + 1; j < enemies.size(); j++) {

            _Enemy* a = enemies[i];
            _Enemy* b = enemies[j];

            if (!a->active || !b->active) {
                continue;
            }

            if (myCollision->checkRadiusHit(a->pos, enemyRadius, b->pos, enemyRadius)) {

                float dx = (float)(b->pos.x - a->pos.x);
                float dz = (float)(b->pos.z - a->pos.z);

                float distSq = dx * dx + dz * dz;

                if (distSq <= 0.0001f) {
                    a->pos.x += 0.1f;
                    b->pos.x -= 0.1f;
                    continue;
                }

                float dist = sqrtf(distSq);

                float nx = dx / dist;
                float nz = dz / dist;

                float minDist = enemyRadius * 2.0f;
                float overlap = minDist - dist;

                a->pos.x -= nx * overlap * 0.5f;
                a->pos.z -= nz * overlap * 0.5f;

                b->pos.x += nx * overlap * 0.5f;
                b->pos.z += nz * overlap * 0.5f;
            }
        }
    }
}

void _SceneJungle::clearEnemies()
{
    for (size_t i = 0; i < enemies.size(); i++) {
        delete enemies[i];
    }

    enemies.clear();
}

void _SceneJungle::clearPortals()
{
    for (size_t i = 0; i < portals.size(); i++) {
        delete portals[i].sprite;
        portals[i].sprite = nullptr;
    }

    portals.clear();
}

void _SceneJungle::removeDeadEnemies()
{
    for (int i = (int)enemies.size() - 1; i >= 0; i--) {
        if (enemies[i]->dead) {
            delete enemies[i];
            enemies.erase(enemies.begin() + i);
        }
    }
}

void _SceneJungle::initMagicBullets()
{
    for (int i = 0; i < MAX_MAGIC_BULLETS; i++) {
        magicBullets[i].initBlt(
            player->pos,
            "images/teapotT.jpg",   // bullet texture
            "models/cube.obj",      // bullet model
            "images/f.png"          // particle texture
        );

        magicBullets[i].scale.x = 0.4f;
        magicBullets[i].scale.y = 0.4f;
        magicBullets[i].scale.z = 0.4f;
    }
}

_Enemy* _SceneJungle::findNearestEnemyInRange(float range)
{
    _Enemy* nearest = nullptr;
    float bestDistSq = range * range;

    for (size_t i = 0; i < enemies.size(); i++) {
        _Enemy* enemy = enemies[i];

        if (!enemy->active || enemy->dead) {
            continue;
        }

        float dx = (float)(enemy->pos.x - player->pos.x);
        float dz = (float)(enemy->pos.z - player->pos.z);

        float distSq = dx * dx + dz * dz;

        if (distSq < bestDistSq) {
            bestDistSq = distSq;
            nearest = enemy;
        }
    }

    return nearest;
}

void _SceneJungle::updateAutoMagic(float deltaT)
{
    player->autoFireTimer += deltaT;

    if (player->autoFireTimer < player->autoFireRate) {
        return;
    }

    player->autoFireTimer = 0.0f;

    _Enemy* target = findNearestEnemyInRange(player->autoFireRange);

    if (!target) {
        return;
    }

    _bullets& bullet = magicBullets[nextBulletIndex];

    vec3 targetPos = target->pos;
    targetPos.y += 1.0f;

    // This is the code you asked about.
    // It initializes the bullet shot.
    bullet.start = player->pos;
    bullet.start.y += 1.0f;

    bullet.pos = bullet.start;
    bullet.dest = targetPos;
    bullet.t = 0.0f;
    bullet.timer = 0.0f;
    bullet.isLive = true;
    bullet.actrigger = bullet.ACTIVE;

    nextBulletIndex++;
    nextBulletIndex = nextBulletIndex % MAX_MAGIC_BULLETS;
}

void _SceneJungle::updateMagicBullets(float deltaT)
{
    for (int i = 0; i < MAX_MAGIC_BULLETS; i++) {
        _bullets& bullet = magicBullets[i];

        if (bullet.actrigger == bullet.ACTIVE) {
            bullet.isLive = true;
            bullet.shoot(bullet.start, bullet.dest, deltaT);
        }

        if (!bullet.isLive) {
            continue;
        }

        for (size_t j = 0; j < enemies.size(); j++) {
            _Enemy* enemy = enemies[j];

            if (!enemy->active || enemy->dead) {
                continue;
            }

            if (myCollision->checkRadiusHit(bullet.pos, 0.6f, enemy->pos, enemyRadius)) {
                enemy->takeDamage(player->bulletDamage);

                bullet.actrigger = bullet.HIT;
                bullet.isLive = false;
                bullet.explosionGenerated = false;
                bullet.explosionTimer = 0.0f;

                break;
            }
        }
    }
}

void _SceneJungle::drawMagicBullets()
{
    for (int i = 0; i < MAX_MAGIC_BULLETS; i++) {
        magicBullets[i].bulletActions();
        magicBullets[i].drawBullet();
    }
}
// ======================================================
// Input
// ======================================================

void _SceneJungle::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_KEYDOWN:
            switch (wParam)
            {
                case 'W':
                case VK_UP:
                    player->setMoveForward(true);
                    break;

                case 'S':
                case VK_DOWN:
                    player->setMoveBackward(true);
                    break;

                case 'A':
                case VK_LEFT:
                    player->setTurnRight(true);
                    break;

                case 'D':
                case VK_RIGHT:
                    player->setTurnLeft(true);
                    break;

                case VK_ESCAPE:
                    if (manager) {
                        manager->requestSceneChange(new _MenuScene());
                    }
                    break;
                case 'P':
                    paused = true;
                    break;
                case 'L':
                    player->playerLives = 0;
                default:
                    break;
            }
            break;

        case WM_KEYUP:
            switch (wParam)
            {
                case 'W':
                case VK_UP:
                    player->setMoveForward(false);
                    break;

                case 'S':
                case VK_DOWN:
                    player->setMoveBackward(false);
                    break;

                case 'A':
                case VK_LEFT:
                    player->setTurnRight(false);
                    break;

                case 'D':
                case VK_RIGHT:
                    player->setTurnLeft(false);
                    break;

                default:
                    break;
            }
            break;
        case WM_LBUTTONDOWN:
            if(gameOver){
                float mouseX = (float)LOWORD(lParam);
                float mouseY = (float)HIWORD(lParam);

                vec3 posExitPanel = exitPanel->pos;
                vec3 posPlayAgainPanel = playAgainPanel->pos;
                vec3 posMenuPanel = menuPanel->pos;
                vec3 posHelpPanel = helpPanel->pos;

                float leftExit   = posExitPanel.x - exitPanel->width * 0.5f;
                float rightExit  = posExitPanel.x + exitPanel->width * 0.5f;
                float topExit    = posExitPanel.y - exitPanel->height * 0.5f;
                float bottomExit = posExitPanel.y + exitPanel->height * 0.5f;

                float leftPlay   = posPlayAgainPanel.x - playAgainPanel->width * 0.5f;
                float rightPlay  = posPlayAgainPanel.x + playAgainPanel->width * 0.5f;
                float topPlay    = posPlayAgainPanel.y - playAgainPanel->height * 0.5f;
                float bottomPlay = posPlayAgainPanel.y + playAgainPanel->height * 0.5f;

                if (mouseX >= leftPlay && mouseX <= rightPlay &&
                    mouseY >= topPlay && mouseY <= bottomPlay)
                {
                    resetGame();
                }
                else if (mouseX >= leftExit && mouseX <= rightExit &&
                         mouseY >= topExit && mouseY <= bottomExit)
                {
                    manager->requestSceneChange(new _MenuScene());
                }
            }
            if(gameWin){
                float mouseX = (float)LOWORD(lParam);
                float mouseY = (float)HIWORD(lParam);

                vec3 posExitPanel = exitPanel->pos;
                vec3 posPlayAgainPanel = playNextPanel->pos;
                vec3 posMenuPanel = menuPanel->pos;
                vec3 posHelpPanel = helpPanel->pos;

                float leftExit   = posExitPanel.x - exitPanel->width * 0.5f;
                float rightExit  = posExitPanel.x + exitPanel->width * 0.5f;
                float topExit    = posExitPanel.y - exitPanel->height * 0.5f;
                float bottomExit = posExitPanel.y + exitPanel->height * 0.5f;

                float leftPlay   = posPlayAgainPanel.x - playAgainPanel->width * 0.5f;
                float rightPlay  = posPlayAgainPanel.x + playAgainPanel->width * 0.5f;
                float topPlay    = posPlayAgainPanel.y - playAgainPanel->height * 0.5f;
                float bottomPlay = posPlayAgainPanel.y + playAgainPanel->height * 0.5f;

                if (mouseX >= leftPlay && mouseX <= rightPlay &&
                    mouseY >= topPlay && mouseY <= bottomPlay)
                {
                    manager->requestSceneChange(new _SceneDungeon());
                }
                else if (mouseX >= leftExit && mouseX <= rightExit &&
                         mouseY >= topExit && mouseY <= bottomExit)
                {
                    PostQuitMessage(0);
                }
            }
            if (paused){
                float mouseX = (float)LOWORD(lParam);
                float mouseY = (float)HIWORD(lParam);

                vec3 posExitPanel = exitPanel->pos;
                vec3 posPlayAgainPanel = playAgainPanel->pos;
                vec3 posMenuPanel = menuPanel->pos;
                vec3 posHelpPanel = helpPanel->pos;

                float leftExit   = posExitPanel.x - exitPanel->width * 0.5f;
                float rightExit  = posExitPanel.x + exitPanel->width * 0.5f;
                float topExit    = posExitPanel.y - exitPanel->height * 0.5f;
                float bottomExit = posExitPanel.y + exitPanel->height * 0.5f;

                float leftPlay   = posPlayAgainPanel.x - playAgainPanel->width * 0.5f;
                float rightPlay  = posPlayAgainPanel.x + playAgainPanel->width * 0.5f;
                float topPlay    = posPlayAgainPanel.y - playAgainPanel->height * 0.5f;
                float bottomPlay = posPlayAgainPanel.y + playAgainPanel->height * 0.5f;

                float leftMenu   = posMenuPanel.x - menuPanel->width * 0.5f;
                float rightMenu  = posMenuPanel.x + menuPanel->width * 0.5f;
                float topMenu    = posMenuPanel.y - menuPanel->height * 0.5f;
                float bottomMenu  = posMenuPanel.y + menuPanel->height * 0.5f;

                float leftHelp   = posHelpPanel.x - helpPanel->width * 0.5f;
                float rightHelp  = posHelpPanel.x + helpPanel->width * 0.5f;
                float topHelp    = posHelpPanel.y - helpPanel->height * 0.5f;
                float bottomHelp  = posHelpPanel.y + helpPanel->height * 0.5f;

                if (mouseX >= leftPlay && mouseX <= rightPlay &&
                    mouseY >= topPlay && mouseY <= bottomPlay)
                {
                    paused = false;
                }
                else if (mouseX >= leftExit && mouseX <= rightExit &&
                         mouseY >= topExit && mouseY <= bottomExit)
                {
                    PostQuitMessage(0);
                    paused = false;
                }
                else if (mouseX >= leftMenu && mouseX <= rightMenu &&
                         mouseY >= topMenu && mouseY <= bottomMenu)
                {
                    manager->requestSceneChange(new _MenuScene());
                    paused = false;
                }
                else if (mouseX >= leftHelp && mouseX <= rightHelp &&
                         mouseY >= topHelp && mouseY <= bottomHelp)
                {
                    manager->requestSceneChange(new _TutorialScene());
                    paused = false;
                }
            }
            break;
        case WM_RBUTTONDOWN:
            break;
        case WM_MBUTTONDOWN: break;
        case WM_LBUTTONUP:
            player->setAttack(false);
            break;
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            break;
        case WM_MOUSEMOVE:
             break;
        case WM_MOUSEWHEEL:
            break;
        default:
            break;
    }
}

// ======================================================
// HUD
// ======================================================

void _SceneJungle::drawLivesHUD()
{
    for (int i = 0; i < player->playerLives; i++) {
        heartIcon->pos.x = 30.0f + i * 60.0f;
        heartIcon->pos.y = 30.0f;
        heartIcon->draw(wWidth, wHeight);
    }
}

void _SceneJungle::drawGameOverHUD()
{
    gameOverPanel->pos.x = wWidth / 2.0f;
    gameOverPanel->pos.y = (wHeight / 2.0f) - (gameOverPanel->height / 2.0f);
    gameOverPanel->draw(wWidth, wHeight);

    exitPanel->pos.x = (wWidth / 2.0f) + 400.0f;
    exitPanel->pos.y = ((wHeight / 2.0f) - (exitPanel->height / 2.0f)) + 300.0f;
    exitPanel->draw(wWidth, wHeight);

    playAgainPanel->pos.x = (wWidth / 2.0f) - 400.0f;
    playAgainPanel->pos.y = ((wHeight / 2.0f) - (playAgainPanel->height / 2.0f)) +300.0f;
    playAgainPanel->draw(wWidth, wHeight);
}

void _SceneJungle::drawGameWinHUD()
{
    gameWinPanel->pos.x = wWidth / 2.0f;
    gameWinPanel->pos.y = (wHeight / 2.0f) - (gameOverPanel->height / 2.0f);
    gameWinPanel->draw(wWidth, wHeight);

    exitPanel->pos.x = (wWidth / 2.0f) + 400.0f;
    exitPanel->pos.y = ((wHeight / 2.0f) - (exitPanel->height / 2.0f)) + 300.0f;
    exitPanel->draw(wWidth, wHeight);

    playNextPanel->pos.x = (wWidth / 2.0f) - 400.0f;
    playNextPanel->pos.y = ((wHeight / 2.0f) - (playAgainPanel->height / 2.0f)) +300.0f;
    playNextPanel->draw(wWidth, wHeight);
}

void _SceneJungle::drawTimerHUD()
{
    int totalSeconds = (int)gameTimeRemaining;

    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    char timerText[32];

    sprintf(timerText, "SURVIVE: %d:%02d", minutes, seconds);

    //many draws to make it look bold
    drawText(125.0+(wWidth/4), wHeight - 84.0f, timerText, 0.8f);
    drawText(125.0+(wWidth/4), wHeight - 85.0f, timerText, 0.8f);
    drawText(125.0+(wWidth/4)+1.0, wHeight - 85.0f, timerText, 0.8f);
    drawText(125.0+(wWidth/4)-1.0, wHeight - 85.0f, timerText, 0.8f);
    drawText(125.0+(wWidth/4)+1.0, wHeight - 84.0f, timerText, 0.8f);
}

void _SceneJungle::pausedHUD()
{
    pausedPanel->pos.x = wWidth / 2.0f;
    pausedPanel->pos.y = (wHeight / 2.0f) - (pausedPanel->height / 2.0f);
    pausedPanel->draw(wWidth, wHeight);

    exitPanel->pos.x = (wWidth / 2.0f) + 600.0f;
    exitPanel->pos.y = ((wHeight / 2.0f) - (exitPanel->height / 2.0f)) + 300.0f;
    exitPanel->draw(wWidth, wHeight);

    playAgainPanel->pos.x = (wWidth / 2.0f) - 600.0f;
    playAgainPanel->pos.y = ((wHeight / 2.0f) - (playAgainPanel->height / 2.0f)) +300.0f;
    playAgainPanel->draw(wWidth, wHeight);

    menuPanel->pos.x = (wWidth / 2.0f) + 200.0f;
    menuPanel->pos.y = ((wHeight / 2.0f) - (menuPanel->height / 2.0f)) +300.0f;
    menuPanel->draw(wWidth, wHeight);

    helpPanel->pos.x = (wWidth / 2.0f) - 200.0f;
    helpPanel->pos.y = ((wHeight / 2.0f) - (helpPanel->height / 2.0f)) +300.0f;
    helpPanel->draw(wWidth, wHeight);
}

void _SceneJungle::resetGame()
{
    player->playerLives = 5;
    player->setDeath(false);
    player->setAttack(false);

    gameOver = false;
    paused = false;

    gameTimeRemaining = 120.0f;
    deathAnimationPeriod = 3.0f;
    enemyDamageCooldown = 0.0f;

    player->pos.x = 0.0f;
    player->pos.y = 0.0f;
    player->pos.z = 0.0f;
    player->yaw = 0.0f;

    clearEnemies();
    clearPortals();

    // Optional: spawn a starter enemy again
    spawnPortals(0.0f, 0.0f, -20.0f, 1);
    spawnEnemy(0.0f, 0.0f, -20.0f, dragonknight);

    cam->followTarget(player->pos, player->yaw, camFollowDistance, camHeight, camLookHeight);
}

void _SceneJungle::drawText(float x, float y, const char* text, float scale)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D(0, wWidth, 0, wHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glColor3f(1.0f, 1.0f, 1.0f);

    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    for (int i = 0; text[i] != '\0'; i++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}
