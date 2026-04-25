#include "_scene.h"
#include "scenemanager.h"
#include "_menuscene.h"
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
    skyBox->myTex[0].loadTexture("images/front.png");
    skyBox->myTex[1].loadTexture("images/back.png");
    skyBox->myTex[2].loadTexture("images/top.png");
    skyBox->myTex[3].loadTexture("images/bottom.png");
    skyBox->myTex[4].loadTexture("images/left.png");
    skyBox->myTex[5].loadTexture("images/right.png");

    // HUD
    heartIcon->initIcon("images/heart.png");
    heartIcon->width = 60.0f;
    heartIcon->height = 60.0f;
    gameOverPanel->initIcon("images/menu/gameover.png");
    gameOverPanel->width = 300.0f;
    gameOverPanel->height = 120.0f;
    playAgainPanel->initIcon("images/menu/newgame.png");
    playAgainPanel->width = 150.0f;
    playAgainPanel->height = 60.0f;
    exitPanel->initIcon("images/menu/exit.png");
    exitPanel->width = 150.0f;
    exitPanel->height = 60.0f;

    // player model
    player->init("models/Tekk/tris.md2", "models/Tekk/blade.jpg");
    player->pos.x = 0.0f;
    player->pos.y = 0.0f;
    player->pos.z = 0.0f;
    player->yaw = 0.0f;

    // world ground
    worldGround->modelInit("images/jungleScene/jungle_road.png");
    worldGround->scale.x = 100.0f;
    worldGround->scale.z = 100.0f;
    worldGround->pos.x = 0.0f;
    worldGround->pos.y = -1.0f;
    worldGround->pos.z = 0.0f;

    cam->followTarget(player->pos, player->yaw, camFollowDistance, camHeight, camLookHeight);
    player->playerLives = 0;
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

    player->update(deltaT);
    cam->followTarget(player->pos, player->yaw, camFollowDistance, camHeight, camLookHeight);

    // --------------------------------------
    // 5. draw gameplay entities
    // --------------------------------------

    player->draw();
}

// ======================================================
// Scene helpers
// ======================================================

void _Scene::drawWorld()
{
    glPushMatrix();
        worldGround->drawModel();
        drawLivesHUD();
    glPopMatrix();
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
                vec3 posPlayPanel = playAgainPanel->pos;

                float left   = posExitPanel.x - exitPanel->width * 0.5f;
                float right  = posExitPanel.x + exitPanel->width * 0.5f;
                float top    = posExitPanel.y - exitPanel->height * 0.5f;
                float bottom = posExitPanel.y + exitPanel->height * 0.5f;

                float left2   = posPlayPanel.x - playAgainPanel->width * 0.5f;
                float right2  = posPlayPanel.x + playAgainPanel->width * 0.5f;
                float top2    = posPlayPanel.y - playAgainPanel->height * 0.5f;
                float bottom2 = posPlayPanel.y + playAgainPanel->height * 0.5f;

                if(mouseX >= left2 && mouseX <= right2 && mouseY >= top2  && mouseY <= bottom2){
                    resetGame();
                }
                if(mouseX >= left && mouseX <= right && mouseY >= top  && mouseY <= bottom){
                    manager->requestSceneChange(new _MenuScene());
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

void _Scene::drawLivesHUD()
{
    for (int i = 0; i < player->playerLives; i++) {
        heartIcon->pos.x = 30.0f + i * 45.0f;
        heartIcon->pos.y = 30.0f;
        heartIcon->draw(wWidth, wHeight);
    }
}

void _Scene::drawGameOverHUD()
{
    gameOverPanel->pos.x = wWidth / 2.0f;
    gameOverPanel->pos.y = (wHeight / 2.0f) - (gameOverPanel->height / 2.0f);
    gameOverPanel->draw(wWidth, wHeight);

    exitPanel->pos.x = (wWidth / 2.0f) +150.0f;
    exitPanel->pos.y = ((wHeight / 2.0f) - (exitPanel->height / 2.0f)) +300.0f;
    exitPanel->draw(wWidth, wHeight);

    playAgainPanel->pos.x = (wWidth / 2.0f) - 150.0f;
    playAgainPanel->pos.y = ((wHeight / 2.0f) - (playAgainPanel->height / 2.0f)) +300.0f;
    playAgainPanel->draw(wWidth, wHeight);
}

void _Scene::resetGame()
{
    player->playerLives = 5;
    gameOver = false;
    player->pos.x = 100;

}
