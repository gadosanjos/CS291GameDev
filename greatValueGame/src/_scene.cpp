#include "_scene.h"
#include "scenemanager.h"
#include "_menuscene.h"
#include "_tutorialscene.h"
#include "_pausedscene.h"
#include <cmath>

// ======================================================
// Construction / destruction
// ======================================================

_Scene::_Scene()
{
    lastTime = chrono::steady_clock::now();
}

_Scene::~_Scene()
{
    delete myLight;
    delete skyBox;
    delete cam;
    delete myMusic;
    delete worldGround;
    delete player;
    delete myCollision;

    for (size_t i = 0; i < enemies.size(); i++) {
        delete enemies[i];
    }
    enemies.clear();
    for (size_t i = 0; i < portals.size(); i++) {
        delete portals[i].sprite;
    }
    portals.clear();
}

// ======================================================
// Core scene lifecycle
// ======================================================

GLint _Scene::initGL()
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
    skyBox->myTex[0].loadTexture("images/back.bmp");
    skyBox->myTex[1].loadTexture("images/front.bmp");
    skyBox->myTex[2].loadTexture("images/top.bmp");
    skyBox->myTex[3].loadTexture("images/bottom.bmp");
    skyBox->myTex[4].loadTexture("images/left.bmp");
    skyBox->myTex[5].loadTexture("images/right.bmp");
    // HUD
    heartIcon->initIcon("images/heart.png");
    heartIcon->width = 60.0f;
    heartIcon->height = 60.0f;
    gameOverPanel->initIcon("images/menu/gameover.png");
    gameOverPanel->width = 1000.0f;
    gameOverPanel->height = 520.0f;
    pausedPanel->initIcon("images/menu/gameover.png");
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

    // player model
    player->init("models/Tekk/tris.md2", "models/Tekk/blade.jpg");
    player->pos.x = 0.0f;
    player->pos.y = 0.0f;
    player->pos.z = 0.0f;
    player->yaw = 0.0f;

    // One enemy initialization so i dont forget
    spawnPortals(0.0f, 0.0f, -20.0f);
    spawnEnemy(0.0f, 0.0f, -20.0f);

    // world ground
    worldGround->modelInit("images/jungleScene/jungle_road.png");
    worldGround->scale.x = 100.0f;
    worldGround->scale.z = 100.0f;
    worldGround->pos.x = 0.0f;
    worldGround->pos.y = -1.0f;
    worldGround->pos.z = 0.0f;

    cam->followTarget(player->pos, player->yaw, camFollowDistance, camHeight, camLookHeight);
    //player->playerLives = 0; // game over debug
    return true;
}

void _Scene::reSize(GLint width, GLint height)
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

float _Scene::deltaT = 0.0f;

void _Scene::drawScene()
{
    // --------------------------------------
    // 1. update frame timing
    // --------------------------------------

    auto currentTime = chrono::steady_clock::now();
    chrono::duration<float> elapsed = currentTime - lastTime;
    _Scene::deltaT = elapsed.count();
    lastTime = currentTime;

    // update game timer
    if (!gameOver) {
        if(paused){
            //
        } else {
            gameTimeRemaining -= deltaT;
        }

        if (gameTimeRemaining <= 0.0f) {
            gameTimeRemaining = 0.0f;
            gameOver = true;
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
        gameOver = true;
    }
    if(gameOver){
        drawGameOverHUD();
        return;
    } else if(player->playerLives >= 10){
        manager->requestSceneChange(new _SceneVirtualWorld());
    }
    if(paused){
        pausedHUD();
        return;
    }
    if(enemies.size() < 25){
        randomizeEnemySpawnPositions(player->pos, 10, 25);
    }

    player->update(deltaT);
    updatePortals(deltaT);
    updateEnemies(deltaT);
    separateEnemies();
    cam->followTarget(player->pos, player->yaw, camFollowDistance, camHeight, camLookHeight);

    // --------------------------------------
    // 5. draw gameplay entities
    // --------------------------------------

    player->draw();
    drawPortals();
    drawEnemies();
}

// ======================================================
// Scene helpers
// ======================================================

void _Scene::drawWorld()
{
    glPushMatrix();
        worldGround->drawModel();
        drawLivesHUD();
        drawTimerHUD();
    glPopMatrix();
}

// ======================================================
// Mob helpers
// ======================================================
void _Scene::randomizeEnemySpawnPositions(vec3 playerPos, int distanceRange, int minDistance)
{
    float angle = ((float)rand() / RAND_MAX) * 2.0f * (float)PI;
    float radius = minDistance + ((float)rand() / RAND_MAX) * distanceRange;

    float spawnX = playerPos.x + cosf(angle) * radius;
    float spawnZ = playerPos.z + sinf(angle) * radius;

    spawnPortals(spawnX, 0.0f, spawnZ-1);
    spawnEnemy(spawnX, 0.0f, spawnZ);
}

void _Scene::spawnPortals(float x, float y, float z)
{
    PortalEffect p;

    p.sprite = new _spritesheet();
    p.sprite->spriteInit("images/blueportal.png", 1, 1);
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

void _Scene::spawnEnemy(float x, float y, float z)
{
    _Enemy* e = new _Enemy();
    e->init("models/Tekk/tris.md2", "models/Tekk/blade.jpg");
    e->pos.x = x;
    e->pos.y = y;
    e->pos.z = z;
    enemies.push_back(e);
}

void _Scene::updatePortals(float deltaT)
{
    for (int i = (int)portals.size() - 1; i >= 0; i--) {
        portals[i].age += deltaT;

        if (portals[i].age >= portals[i].lifetime) {
            delete portals[i].sprite;
            portals.erase(portals.begin() + i);
        }
    }
}

void _Scene::updateEnemies(float deltaT)
{
    for (size_t i = 0; i < enemies.size(); i++) {
        enemies[i]->update(deltaT, player->pos);
    }
}

void _Scene::drawPortals()
{
    for (size_t i = 0; i < portals.size(); i++) {
        if (portals[i].sprite) {
            portals[i].sprite->drawSprite();
        }
    }
}

void _Scene::drawEnemies()
{
    for (size_t i = 0; i < enemies.size(); i++) {
        enemies[i]->draw();
    }
}

void _Scene::separateEnemies()
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

// ======================================================
// Input
// ======================================================

void _Scene::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
            // Copy this entire if statement.
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
                    //resetGame();
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

void _Scene::drawLivesHUD()
{
    for (int i = 0; i < player->playerLives; i++) {
        heartIcon->pos.x = 30.0f + i * 60.0f;
        heartIcon->pos.y = 30.0f;
        heartIcon->draw(wWidth, wHeight);
    }
}

void _Scene::drawGameOverHUD()
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

void _Scene::drawTimerHUD()
{
    int totalSeconds = (int)gameTimeRemaining;

    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    char timerText[32];

    sprintf(timerText, "SURVIVE: %d:%02d", minutes, seconds);

    //many draws to make it look bold
    drawText(wWidth/2, wHeight - 79.0f, timerText, 0.25f);
    drawText(wWidth/2, wHeight - 80.0f, timerText, 0.25f);
    drawText((wWidth/2)+1.0, wHeight - 80.0f, timerText, 0.25f);
    drawText((wWidth/2)-1.0, wHeight - 80.0f, timerText, 0.25f);
    drawText((wWidth/2)+1.0, wHeight - 79.0f, timerText, 0.25f);
}

void _Scene::pausedHUD()
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

void _Scene::resetGame()
{
    player->playerLives = 5;
    gameOver = false;
    gameTimeRemaining = 300.0f;

    player->pos.x = 100;

    cam->followTarget(player->pos, player->yaw, camFollowDistance, camHeight, camLookHeight);
}

void _Scene::drawText(float x, float y, const char* text, float scale)
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
