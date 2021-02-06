#pragma once

#include "SDL2/SDL.h"
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "core/log.h"
#include <stdexcept>

namespace Bubble
{
    class Window
    {
        SDL_Window* mWindow;
        SDL_GLContext mGLContext;
        const char* glsl_version = "#version 420";
        bool mIsOpen = true;

    public:
        Window();
        ~Window();

        bool IsOpen();
        
        int GetWidth();
        int GetHeight();
        glm::ivec2 GetSize();

        SDL_Window* GetWindow();
        SDL_GLContext GetGLContext();
        const char* GetGLSLVersion();

        bool PollEvent(SDL_Event& event);
        void OnEvent(SDL_Event& event);

        void OnUpdate();
        void Close();

        void SetVSync(bool mode);
    };
}