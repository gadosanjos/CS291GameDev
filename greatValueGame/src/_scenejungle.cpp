#include "_scenejungle.h"
#include "_scenevirtualworld.h"
#include "_scene.h"
#include "scenemanager.h"
#include "_menuscene.h"

// ======================================================
// Construction / destruction
// ======================================================
_SceneJungle::_SceneJungle()
{
    //ctor
    lastTime = chrono::steady_clock::now();
}

_SceneJungle::~_SceneJungle()
{
    delete myLight;
    delete skyBox;
    delete cam;
    delete testCube;
    delete mdl;
    delete mdlW;
    delete box;
    delete coinSprite;
    delete keyMS;
    delete myMusic;
    delete myCollision;
    delete heartIcon;
    delete coinIcon;
    delete gameOverPanel;

    for (int i = 0; i < NUM_ROAD_CHUNKS; i++) {
        delete road[i];
        road[i] = nullptr;
    }
}

// ======================================================
// Core scene lifecycle
// ======================================================

GLint _SceneJungle::initGL()
{
    glewInit();

    wWidth  = GetSystemMetrics(SM_CXSCREEN);
    wHeight = GetSystemMetrics(SM_CYSCREEN);

    glClearColor(0.77,0.63,0.44,1.0); // setting background color
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);

    myLight->setLight(GL_LIGHT0);

    myMusic->iniSounds();
    Sleep(250);
    myMusic->playMusic(_SceneJungle::song);

    skyBox->boxInit();
    skyBox->myTex[0].loadTexture("images/jungleScene/back.png");
    skyBox->myTex[1].loadTexture("images/jungleScene/back.png");
    skyBox->myTex[2].loadTexture("images/jungleScene/back.png");
    skyBox->myTex[3].loadTexture("images/jungleScene/back.png");
    skyBox->myTex[4].loadTexture("images/jungleScene/back.png");
    skyBox->myTex[5].loadTexture("images/jungleScene/back.png");
    Sleep(250);

    heartIcon->initIcon("images/heart.png");
    heartIcon->width = 60.0f;
    heartIcon->height = 60.0f;

    coinSprite->spriteInit("images/coin.png", 12, 1); // example: 6 frames in 1 row
    coinSprite->actionTrigger = coinSprite->WALKLEFT; // temporary reuse solution
    coinSprite->scale.x = 0.6f;
    coinSprite->scale.y = 0.6f;
    Sleep(250);

    coinIcon->initIcon("images/coins.png");
    coinIcon->width = 60.0f;
    coinIcon->height = 60.0f;
    gameOverPanel->initIcon("images/gameover.png");
    gameOverPanel->width = 300.0f;
    gameOverPanel->height = 120.0f;
    playAgainPanel->initIcon("images/newgame.png");
    playAgainPanel->width = 150.0f;
    playAgainPanel->height = 60.0f;
    exitPanel->initIcon("images/exit.png");
    exitPanel->width = 150.0f;
    exitPanel->height = 60.0f;
    Sleep(250);

    // Player
    mdl->initModel("models/Tekk/tris.md2","models/Tekk/blade.jpg");
    mdl->actionTrigger = mdl->RUN;
    mdlW->initModel("models/Tekk/weapon.md2","models/Tekk/blade.jpg");

    box->modelInit("images/jungleScene/crate.png");
    Sleep(250);
    // Road
    for(int i = 0; i < _SceneJungle::numRoadChunks; i++){
        road[i] = new _ground();
        //DEBUG FOR VISUALIZATION
        if(i%2 == 0){
            road[i]->modelInit("images/jungleScene/jungle_road.png");
        } else {
            road[i]->modelInit("images/jungleScene/jungle_road.png");
            //road[i]->modelInit("images/crate.jpeg");
        }
        road[i]->pos.z -= (30*i);


        // Obstacles
        // Make first chunk empty only at startup
        if (i == 0 || i == 1) {
            clearChunkObstacles(i);
        } else {
            generateChunkObstacles(i);
        }
    }
    // Test cube
    testCube->initModel("images/debug.jpg");

    return true;
}

void _SceneJungle::reSize(GLint width, GLint height)
{
    GLfloat aspectRatio = (GLfloat)width/(GLfloat)height;
    glViewport(0,0,width,height); //adjusting the viewport
    glMatrixMode(GL_PROJECTION);  //for perspective projection
    glLoadIdentity();

    gluPerspective(60,aspectRatio,0.1,2000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
float _SceneJungle::deltaT = 0;

void _SceneJungle::drawScene()
{
    // --------------------------------------
    // 1. update frame timing
    // --------------------------------------

    auto currentTime = chrono::steady_clock::now();

    chrono::duration<float>elapsed = currentTime - lastTime;
    _SceneJungle::deltaT = elapsed.count();
    lastTime =currentTime;;

    // --------------------------------------
    // 2. clear frame and set camera
    // --------------------------------------

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    cam->setUpCamera();

    // --------------------------------------
    // 3. draw background / sky
    // --------------------------------------

    glPushMatrix();
        skyBox->drawBox();
        skyBox->rot.x +=0.5;
    glPopMatrix();

    // --------------------------------------
    // 4. update and draw world
    // --------------------------------------

    if (gameOver) {
        drawGameOverHUD();
        glPushMatrix();
            //testCube->drawModel();
            testCube->pos.x = laneX[currentPlayerLane];
        glPopMatrix();

        glPushMatrix();
            glTranslatef(laneX[LEFT]*2, 0, -15);
            glRotatef(-90,1,0,0);
            glRotatef(270,0,0,1);
            glScalef(0.3,0.3,0.3);
            mdl->Draw();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(laneX[RIGHT]*2, 0, -15);
            glRotatef(-90,1,0,0);
            glRotatef(270,0,0,1);
            glScalef(0.3,0.3,0.3);
            mdl->Draw();
        glPopMatrix();
        return;
    } else if (coinCount == 25){
        manager->requestSceneChange(new _Scene());
    }

    // Move and draw road first
    roadChunk(_SceneJungle::deltaT);

    // --------------------------------------
    // 5. draw gameplay entities
    // --------------------------------------

    // animate the coin sprite
    coinSprite->spriteActions(deltaT);

    // Then draw obstacles attached to the updated road positions
    for (int i = 0; i < _SceneJungle::numRoadChunks; i++) {
        //drawTestObstacles(i);
        drawObstacles(i);
        drawCoins(i);
    }

    glPushMatrix();
        //testCube->drawModel();
        testCube->pos.x = laneX[currentPlayerLane];
    glPopMatrix();

    glPushMatrix();
        glTranslatef(laneX[currentPlayerLane], 0, 0);
        glRotatef(-90,1,0,0);
        glRotatef(90,0,0,1);
        glScalef(0.03,0.03,0.03);
        mdl->actions();
        mdl->Draw();
        //mdlW->Draw();
        //mdl->pos.x = laneX[currentPlayerLane]*10;
        //mdlW->pos.x = laneX[currentPlayerLane];
    glPopMatrix();

    // --------------------------------------
    // 6. draw HUD
    // --------------------------------------

    drawLivesHUD();
    drawCoinHUD();

    // --------------------------------------
    // 7. draw game over overlay
    // --------------------------------------
}

void _SceneJungle::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
            case 'A':
            case VK_LEFT:
                if (currentPlayerLane > LEFT)
                    currentPlayerLane = static_cast<Lane>(currentPlayerLane - 1);
                break;
            case 'D':
            case VK_RIGHT:
                if (currentPlayerLane < RIGHT)
                    currentPlayerLane = static_cast<Lane>(currentPlayerLane + 1);
                break;
            case VK_RETURN:
                if (gameOver){
                    resetGame();
                }
                break;
            case '0': // debug for checking scene change
            case VK_ESCAPE:
                    cout << "here" << endl;
                    manager->requestSceneChange(new _MenuScene());
                break;
            default:
                break;
        }
        keyMS->wParam = wParam;
        keyMS->keyPressed(testCube);
        keyMS->keyPressed(box);
        //keyMS->keyPressed(skyBox);
        //keyMS->keyPressed(cam); // camera should be fixed
        //keyMS->keyPressed(mySprite);
        keyMS->keyPressed(mdl,mdlW);
        break;

    case WM_KEYUP:
         //mySprite->actionTrigger = mySprite->STAND;
         break;

    case WM_LBUTTONDOWN:
            // after game over
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
    case WM_RBUTTONDOWN:
         keyMS->wParam = wParam;
         keyMS->mouseEvenDown(LOWORD(lParam),HIWORD(lParam));

            blts[clickCount].shoot(mdl->pos,mouse,deltaT);
            blts[clickCount].actionTrigger= blts[clickCount].ACTIVE;

            clickCount++;
            clickCount = (clickCount)%20;

      //      snds->playSounds("sounds/laser.mp3");`
        break;

    case WM_MBUTTONDOWN: break;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
          keyMS->mouseUp();
          break;

    case WM_MOUSEMOVE:
         //keyMS->mouseMove(skyBox,LOWORD(lParam),HIWORD(lParam));
         //keyMS->mouseMove(testCube,LOWORD(lParam),HIWORD(lParam));
         //keyMS->mouseMove(box,LOWORD(lParam),HIWORD(lParam));
         //
         break;

    case WM_MOUSEWHEEL:
        //keyMS->mouseWheel(testCube,(double)GET_WHEEL_DELTA_WPARAM(wParam));
        keyMS->mouseWheel(box,(double)GET_WHEEL_DELTA_WPARAM(wParam));
        keyMS->mouseWheel(cam,(double)GET_WHEEL_DELTA_WPARAM(wParam));
        break;
    }
}

// ======================================================
// Game flow
// ======================================================

void _SceneJungle::resetGame()
{
    playerLives = 5;
    coinCount = 0;
    gameOver = false;
    currentPlayerLane = MIDDLE;

    for (int i = 0; i < NUM_ROAD_CHUNKS; i++) {
        road[i]->pos.z = -(roadChunkLength * i);

        if (i == 0 || i == 1) {
            clearChunkObstacles(i);
        } else {
            generateChunkObstacles(i);
        }

        for (int row = 0; row < ROWS_PER_CHUNK; row++) {
            for (int lane = 0; lane < LANES; lane++) {
                obstacleHit[i][row][lane] = false;
                coinCollected[i][row][lane] = false;
            }
        }
    }
}

// ======================================================
// Road / world update
// ======================================================

void _SceneJungle::roadChunk(float deltaT)
{
    float dz = _SceneJungle::speed * deltaT;

    for (int i = 0; i < numRoadChunks; i++) {
        road[i]->moveZ(dz);

        if (road[i]->pos.z > _SceneJungle::recyclePoint) {
            road[i]->moveZ(-_SceneJungle::totalLoopLength);

            // new obstacle pattern for recycled chunk
            generateChunkObstacles(i);
            // clear hit flags
            for (int row = 0; row < ROWS_PER_CHUNK; row++) {
                for (int lane = 0; lane < LANES; lane++) {
                    obstacleHit[i][row][lane] = false;
                    coinCollected[i][row][lane] = false;
                }
            }
        }
        road[i]->drawModel();
    }
}

// ======================================================
// Obstacles
// ======================================================

void _SceneJungle::drawObstacles(int chunkIndex)
{
    float obstacleY = -0.4f;
    float localRowZ[ROWS_PER_CHUNK] = { -12.0f, -4.0f, 4.0f, 12.0f };

    for (int row = 0; row < ROWS_PER_CHUNK; row++) {
        for (int lane = 0; lane < LANES; lane++) {
            if (chunkObstacles[chunkIndex].grid[row][lane] == 1) {
                box->pos.x = laneX[lane];
                box->pos.y = obstacleY;
                box->pos.z = road[chunkIndex]->pos.z + localRowZ[row];
                glPushMatrix();
                    box->drawModel();
                    box->rot.y +=0.5;
                    box->rot.x +=0.5;
                glPopMatrix();
                if (!obstacleHit[chunkIndex][row][lane] &&
                    !gameOver &&
                    myCollision->checkLaneHit(currentPlayerLane, lane, playerZ, box->pos.z, hitDistance))
                {
                    obstacleHit[chunkIndex][row][lane] = true;
                    playerLives--;

                    std::cout << "HIT! lives left = " << playerLives << std::endl;

                    if (playerLives <= 0) {
                        gameOver = true;
                        std::cout << "GAME OVER" << std::endl;
                    }
                }
            }
        }
    }
}

void _SceneJungle::drawCoins(int chunkIndex)
{
    float coinY = -0.2f;
    float localRowZ[ROWS_PER_CHUNK] = { -12.0f, -8.0f, -4.0f, 4.0f, 8.0f, 12.0f };

    for (int row = 0; row < ROWS_PER_CHUNK; row++) {
        for (int lane = 0; lane < LANES; lane++) {
            if (chunkObstacles[chunkIndex].grid[row][lane] == 2 &&
                !coinCollected[chunkIndex][row][lane]) {

                coinSprite->pos.x = laneX[lane];
                coinSprite->pos.y = coinY;
                coinSprite->pos.z = road[chunkIndex]->pos.z + localRowZ[row];

                coinSprite->drawSprite();

                if (myCollision->checkLaneHit(currentPlayerLane, lane, playerZ, coinSprite->pos.z, hitDistance)) {
                    coinCollected[chunkIndex][row][lane] = true;
                    coinCount++;
                    std::cout << "Coins: " << coinCount << std::endl;
                }
            }
        }
    }
}

void _SceneJungle::generateChunkObstacles(int chunkIndex)
{
    clearChunkObstacles(chunkIndex);

    for (int row = 0; row < ROWS_PER_CHUNK; row++) {
        int lane = rand() % LANES;
        int roll = rand() % 100;

        if (roll < 45) {
            chunkObstacles[chunkIndex].grid[row][lane] = 1; // obstacle
        }
        else if (roll < 75) {
            chunkObstacles[chunkIndex].grid[row][lane] = 2; // coin
        }
    }
}

void _SceneJungle::clearChunkObstacles(int chunkIndex)
{
    for (int row = 0; row < ROWS_PER_CHUNK; row++) {
        for (int lane = 0; lane < LANES; lane++) {
            chunkObstacles[chunkIndex].grid[row][lane] = 0;
        }
    }
}

// ======================================================
// Debug helpers
// ======================================================

void _SceneJungle::drawTestObstacles(int chunkIndex)
{
    // Height where the obstacle boxes will sit.
    float obstacleY = -0.4f;

    // These are the z positions of each logical obstacle row
    // INSIDE one single road chunk.
    // these are NOT world positions by themselves.
    // They are local offsets relative to the center of the road chunk.
    // Since one road chunk is length 30, its center covers:
    // center - 15  to  center + 15
    // So these rows are being placed inside that chunk at:
    // -12, -4, +4, +12 from the chunk center.
    // Then the actual obstacle z positions become:
    //    row 0  -30 + (-12) = -42
    //    row 1  -30 + (-4) = -34
    //    row 2  -30 + 4 = -26
    //    row 3  -30 + 12 = -18
    float localRowZ[4] = { -12.0f, -4.0f, 4.0f, 12.0f };
    // Loop through obstacle rows.
    // In this test: 4 rows total.
    for (int row = 0; row < 4; row++) {
        // Loop through the 3 lanes:
        // lane 0 = left x = -2.5
        // lane 1 = middle x = 0
        // lane 2 = right x = 2.5
        for (int lane = 0; lane < 3; lane++) {
            // testGrid[row][lane] tells us whether this cell should contain an obstacle box or not.
            // 1 = place a box here
            // 0 = leave this spot empty
            // Example:
            // if testGrid[0][1] == 1,
            // then row 0, middle lane gets a box.
            if (testGrid[row][lane] == 1) {
                // Set the box x position based on which lane it belongs to.
                // laneX[0] might be left lane x
                // laneX[1] might be middle lane x
                // laneX[2] might be right lane x
                box->pos.x = laneX[lane];
                box->pos.y = obstacleY;
                // road[chunkIndex]->pos.z = the CENTER z position of the current road chunk
                // localRowZ[row] = where inside that chunk this obstacle row should sit
                // final obstacle z = chunk center z + local row offset
                box->pos.z = road[chunkIndex]->pos.z + localRowZ[row];
                box->drawModel();
                if (chunkIndex == 0 && row == 0 && lane == 1) {
                    std::cout << "road z: " << road[chunkIndex]->pos.z
                              << " obstacle z: " << box->pos.z
                              << " local offset: " << localRowZ[row]
                              << std::endl;
                }
            }
        }
    }
}

void _SceneJungle::mouseMapping(int x, int y)
{
   GLint viewPort[4];      // for the window
   GLdouble modelViewM[16]; // model and camera
   GLdouble projectionM[16];// for the projectio
   GLfloat winX,winY,winZ;  //mouse clicks

   glGetDoublev(GL_MODELVIEW_MATRIX,modelViewM);
   glGetDoublev(GL_PROJECTION_MATRIX,projectionM);
   glGetIntegerv(GL_VIEWPORT,viewPort);

   winX = (GLfloat)x;
   winY = (GLfloat)viewPort[3] -y;

   glReadPixels(x,(int)winY,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&winZ);

   gluUnProject(winX,winY,winZ,modelViewM,projectionM,viewPort,&mouse.x,&mouse.y,&mouse.z);
}

// ======================================================
// HUD
// ======================================================

void _SceneJungle::drawLivesHUD()
{
    for (int i = 0; i < playerLives; i++) {
        heartIcon->pos.x = 30.0f + i * 45.0f;
        heartIcon->pos.y = 30.0f;
        heartIcon->draw(wWidth, wHeight);
    }
}

void _SceneJungle::drawCoinHUD()
{
    coinIcon->pos.x = 30.0f;
    coinIcon->pos.y = 80.0f;
    coinIcon->draw(wWidth, wHeight);
}

void _SceneJungle::drawGameOverHUD()
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
