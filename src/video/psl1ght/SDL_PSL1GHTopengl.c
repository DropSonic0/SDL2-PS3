/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2020 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_PSL1GHT && SDL_VIDEO_OPENGL

#include "SDL_psl1ghtvideo.h"
#include "SDL_psl1ghtopengl.h"
#include "SDL_loadso.h"

int PSL1GHT_GL_LoadLibrary(_THIS, const char *path)
{
    void *handle;

    if (_this->gl_data) {
        return SDL_SetError("OpenGL context already created");
    }

    /* Load the OpenGL library */
    if (path == NULL) {
        path = "libRSXGL.so";
    }
    _this->gl_config.dll_handle = SDL_LoadObject(path);
    if (!_this->gl_config.dll_handle) {
        return -1;
    }
    SDL_strlcpy(_this->gl_config.driver_path, path,
                SDL_arraysize(_this->gl_config.driver_path));

    /* Allocate OpenGL memory */
    _this->gl_data =
        (struct SDL_GLDriverData *) SDL_calloc(1,
                                               sizeof(struct
                                                      SDL_GLDriverData));
    if (!_this->gl_data) {
        return SDL_OutOfMemory();
    }

    _this->gl_data->egl_data = (struct SDL_EGL_VideoData *) SDL_calloc(1, sizeof(SDL_EGL_VideoData));
    if (!_this->gl_data->egl_data) {
        return SDL_OutOfMemory();
    }

    return 0;
}

void *
PSL1GHT_GL_GetProcAddress(_THIS, const char *proc)
{
    return SDL_EGL_GetProcAddress(_this, proc);
}

void
PSL1GHT_GL_UnloadLibrary(_THIS)
{
    SDL_EGL_UnloadLibrary(_this);
}

SDL_GLContext
PSL1GHT_GL_CreateContext(_THIS, SDL_Window * window)
{
    return SDL_EGL_CreateContext(_this, window);
}

int
PSL1GHT_GL_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context)
{
    return SDL_EGL_MakeCurrent(_this, window, context);
}

int
PSL1GHT_GL_SetSwapInterval(_THIS, int interval)
{
    return SDL_EGL_SetSwapInterval(_this, interval);
}

int
PSL1GHT_GL_GetSwapInterval(_THIS)
{
    return SDL_EGL_GetSwapInterval(_this);
}

int
PSL1GHT_GL_SwapWindow(_THIS, SDL_Window * window)
{
    return SDL_EGL_SwapWindow(_this, window);
}

void
PSL1GHT_GL_DeleteContext(_THIS, SDL_GLContext context)
{
    SDL_EGL_DeleteContext(_this, context);
}

#endif /* SDL_VIDEO_DRIVER_PSL1GHT && SDL_VIDEO_OPENGL */
