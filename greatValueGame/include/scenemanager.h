#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "_common.h"
#include "iscene.h"
#include "_scene.h"
#include "_landingscene.h"
#include "_scenevirtualworld.h"
#include "_scenejungle.h"

class SceneManager
{
    public:
        SceneManager();
        virtual ~SceneManager();

        IScene* currentScene = nullptr;
        IScene* pendingScene = nullptr;

        void changeScene(IScene* newScene); // immediate/internal use
        void requestSceneChange(IScene* newScene);   // safe public request

        GLint initGL();
        void reSize(GLint width, GLint height);
        void drawScene();
        void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


    protected:

    private:
};

#endif // SCENEMANAGER_H
