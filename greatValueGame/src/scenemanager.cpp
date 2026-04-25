#include "scenemanager.h"

SceneManager::SceneManager()
{
    //ctor
    //currentScene = new _LandingScene();
    currentScene = new _Scene();
    //currentScene = new _SceneVirtualWorld();
    //currentScene = new _SceneJungle();

    currentScene->setManager(this);
}

SceneManager::~SceneManager()
{
    //dtor
    delete currentScene;
}
void SceneManager::changeScene(IScene* newScene)
{
    if (currentScene) {
        delete currentScene;
    }

    currentScene = newScene;
    currentScene->setManager(this);
    currentScene->initGL();
}

void SceneManager::requestSceneChange(IScene* newScene)
{
    // If there is already a queued scene, clean it up first
    if (pendingScene) {
        delete pendingScene;
    }
    pendingScene = newScene;
}

GLint SceneManager::initGL()
{
    if(currentScene){
        return currentScene->initGL();
    }
}

void SceneManager::reSize(GLint width, GLint height)
{
    if(currentScene){
        currentScene->reSize(width,height);
    }
}

void SceneManager::drawScene()
{
    if (pendingScene) {
        changeScene(pendingScene);
        pendingScene = nullptr;
    }

    if (currentScene) {
        currentScene->drawScene();
    }
}

void SceneManager::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(currentScene){
        currentScene->WndProc(hWnd,uMsg,wParam,lParam);
    }
}
