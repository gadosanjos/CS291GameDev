#ifndef ISCENE_H
#define ISCENE_H

#include "_common.h"

class SceneManager;

// Scene Interface
class IScene
{
    public:
        IScene() {}
            virtual ~IScene() = default;

            virtual GLint initGL() = 0;
            virtual void reSize(GLint width, GLint height) = 0;
            virtual void drawScene() = 0;
            virtual void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

            void setManager(SceneManager* newManager)
            {
                manager = newManager;
            }

            SceneManager* manager = nullptr;
    protected:

    private:
};

#endif // ISCENE_H
