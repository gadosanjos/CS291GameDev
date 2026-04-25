#ifndef _SCENE_H
#define _SCENE_H

#include<_common.h>
#include<_lighting.h>
#include<_model.h>
#include<_inputs.h>
#include<_modelvbo.h>
#include<_texloader.h>
#include<_skybox.h>
#include<_camera.h>
#include<_parallax.h>
#include<_spritesheet.h>
#include<_sounds.h>
#include<_ModelLoaderMD2.h>
#include<_bullets.h>
#include "iscene.h"
#include "_ground.h"
#include "_collision.h"
#include "_hudicon.h"

class _Scene : public IScene
{
    public:
        _Scene();         //constructor
        virtual ~_Scene() override;//destructor

        // IScene interface
        GLint initGL() override;    // Initializing my game objects and opengl
        void reSize(GLint,GLint) override; //window resizing
        void drawScene() override;  //rendering the scene
        void WndProc(
              HWND	hWnd,			// Handle For This Window
			  UINT	uMsg,			// Message For This Window
			  WPARAM wParam,		// Additional Message Information
			  LPARAM lParam) override;		// Additional Message Information

        // game flow
        void resetGame();

        // world / gameplay helpers
        void roadChunk(float deltaT);
        void drawCoins(int chunkIndex);
        void drawObstacles(int chunkIndex);
        void generateChunkObstacles(int chunkIndex);
        void clearChunkObstacles(int chunkIndex);

        // HUD
        void drawLivesHUD();
        void drawCoinHUD();
        void drawGameOverHUD();
        void mouseMapping(int,int);

        // chunk / lane constants
        static const int LANES = 3;
        static const int ROWS_PER_CHUNK = 6;
        static const int NUM_ROAD_CHUNKS = 4;

        // rendering / scene objects
        _lighting *myLight = new _lighting(); // light class instance
        _skyBox *skyBox = new _skyBox();
        _camera *cam = new _camera();
        _ground* road[NUM_ROAD_CHUNKS];

        // player / gameplay visuals
        _model *testCube = new _model();
        _ModelLoaderMD2 *mdl= new _ModelLoaderMD2();
        _ModelLoaderMD2 * mdlW = new _ModelLoaderMD2(); // waepon
        _modelVBO *box= new _modelVBO();
        _spritesheet *coinSprite = new _spritesheet();
        _bullets blts[20];

        // systems / helpers
        _inputs *keyMS  = new _inputs();
        _sounds *myMusic = new _sounds();
        _collision *myCollision = new _collision();

        // HUD
        _hudIcon* heartIcon = new _hudIcon();
        _hudIcon* coinIcon = new _hudIcon();
        _hudIcon* gameOverPanel = new _hudIcon();
        _hudIcon* playAgainPanel = new _hudIcon();
        _hudIcon* exitPanel = new _hudIcon();

        // Game state
        int playerLives = 5;
        int coinCount = 0;
        bool gameOver = false;
        float playerZ = 0.0f;     // player is fixed at 0
        float hitDistance = 1.0f; // tweak by testing
        int clickCount =0;

        enum Lane {LEFT,MIDDLE,RIGHT};
        Lane currentPlayerLane = MIDDLE;
        float laneX[3] = { -2.5f, 0.0f, 2.5f }; //mapping to x pos

        // road / movement state
        float speed = 10.0f;
        float recyclePoint = 30.0f;
        float roadChunkLength = 30.0f;
        int numRoadChunks = NUM_ROAD_CHUNKS;
        float totalLoopLength = roadChunkLength * numRoadChunks;
        float rowSpacing = 6.0f;

        // Obstacle management
        struct ChunkObstacleData
        {
            int grid[ROWS_PER_CHUNK][LANES];
        };
        // generated obstacle data per chunk
        ChunkObstacleData chunkObstacles[NUM_ROAD_CHUNKS];
        // prevent multiple hits from the same obstacle
        bool obstacleHit[NUM_ROAD_CHUNKS][ROWS_PER_CHUNK][LANES] = {};
        bool coinCollected[NUM_ROAD_CHUNKS][ROWS_PER_CHUNK][LANES] = {};

        // debug / Temp

        // misc scene state
        static float deltaT;
        float wWidth, wHeight;
        vec3 mouse;
        char* song = "sounds/Conspiracy Theory.mp3";
    protected:
    private:
        std::chrono::steady_clock::time_point lastTime;
};

#endif // _SCENE_H
