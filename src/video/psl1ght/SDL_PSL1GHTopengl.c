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

#include "SDL_PSL1GHTvideo.h"
#include "SDL_PSL1GHTopengl.h"

#if SDL_VIDEO_OPENGL_EGL || SDL_VIDEO_OPENGL_PSGL

#if SDL_VIDEO_OPENGL_EGL
#include "SDL_egl_c.h"
#endif

#if SDL_VIDEO_OPENGL_PSGL
#include <PSGL/psgl.h>
#include <PSGL/psglu.h>

// Sony PSGL specific constants from headers
#ifndef GL_ARGB_SCE
#define GL_ARGB_SCE 0x6007
#endif
#ifndef GL_MULTISAMPLING_NONE_SCE
#define GL_MULTISAMPLING_NONE_SCE 0x6030
#endif

#endif

int
PSL1GHT_GL_LoadLibrary(_THIS, const char *path)
{
#if SDL_VIDEO_OPENGL_PSGL
    static int psgl_initialized = 0;
    if (psgl_initialized) return 0;

    PSGLinitOptions options;
    SDL_memset(&options, 0, sizeof(options));
    options.enable = PSGL_INIT_MAX_SPUS | PSGL_INIT_HOST_MEMORY_SIZE;
    options.maxSPUs = 1;
    options.hostMemorySize = 128 * 1024 * 1024; // 128MB

    psglInit(&options);
    psgl_initialized = 1;
    return 0;
#elif SDL_VIDEO_OPENGL_EGL
    return SDL_EGL_LoadLibrary(_this, path, EGL_DEFAULT_DISPLAY, 0);
#else
    return SDL_Unsupported();
#endif
}

void *
PSL1GHT_GL_GetProcAddress(_THIS, const char *proc)
{
#if SDL_VIDEO_OPENGL_PSGL
    // PSGL doesn't have a getProcAddress, functions are usually linked statically
    return NULL;
#elif SDL_VIDEO_OPENGL_EGL
    return SDL_EGL_GetProcAddress(_this, proc);
#else
    return NULL;
#endif
}

void
PSL1GHT_GL_UnloadLibrary(_THIS)
{
#if SDL_VIDEO_OPENGL_PSGL
    psglExit();
#elif SDL_VIDEO_OPENGL_EGL
    SDL_EGL_UnloadLibrary(_this);
#endif
}

SDL_GLContext
PSL1GHT_GL_CreateContext(_THIS, SDL_Window * window)
{
#if SDL_VIDEO_OPENGL_PSGL
    SDL_DeviceData *devdata = (SDL_DeviceData *) _this->driverdata;
    if (!devdata->psgl_device) {
        devdata->psgl_device = psglCreateDeviceAuto(GL_ARGB_SCE, GL_DEPTH_COMPONENT24, GL_MULTISAMPLING_NONE_SCE);
        if (!devdata->psgl_device) {
            SDL_SetError("psglCreateDeviceAuto failed");
            return NULL;
        }
    }
    PSGLcontext* context = psglCreateContext();
    if (!context) {
        SDL_SetError("psglCreateContext failed");
        return NULL;
    }
    return (SDL_GLContext)context;
#elif SDL_VIDEO_OPENGL_EGL
    return SDL_EGL_CreateContext(_this, ((SDL_WindowData *) window->driverdata)->egl_surface);
#else
    return NULL;
#endif
}

int
PSL1GHT_GL_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context)
{
#if SDL_VIDEO_OPENGL_PSGL
    SDL_DeviceData *devdata = (SDL_DeviceData *) _this->driverdata;
    psglMakeCurrent((PSGLcontext*)context, devdata->psgl_device);
    return 0;
#elif SDL_VIDEO_OPENGL_EGL
    return SDL_EGL_MakeCurrent(_this, window ? ((SDL_WindowData *) window->driverdata)->egl_surface : EGL_NO_SURFACE, context);
#else
    return SDL_Unsupported();
#endif
}

int
PSL1GHT_GL_SetSwapInterval(_THIS, int interval)
{
#if SDL_VIDEO_OPENGL_PSGL
    // PSGL doesn't seem to have a direct swap interval API in the provided headers
    return 0;
#elif SDL_VIDEO_OPENGL_EGL
    return SDL_EGL_SetSwapInterval(_this, interval);
#else
    return SDL_Unsupported();
#endif
}

int
PSL1GHT_GL_GetSwapInterval(_THIS)
{
#if SDL_VIDEO_OPENGL_PSGL
    return 0;
#elif SDL_VIDEO_OPENGL_EGL
    return SDL_EGL_GetSwapInterval(_this);
#else
    return 0;
#endif
}

void
PSL1GHT_GL_SwapWindow(_THIS, SDL_Window * window)
{
#if SDL_VIDEO_OPENGL_PSGL
    psglSwap();
#elif SDL_VIDEO_OPENGL_EGL
    SDL_EGL_SwapBuffers(_this, ((SDL_WindowData *) window->driverdata)->egl_surface);
#endif
}

void
PSL1GHT_GL_DeleteContext(_THIS, SDL_GLContext context)
{
#if SDL_VIDEO_OPENGL_PSGL
    psglDestroyContext((PSGLcontext*)context);
#elif SDL_VIDEO_OPENGL_EGL
    SDL_EGL_DeleteContext(_this, context);
#endif
}

#endif /* SDL_VIDEO_OPENGL_EGL || SDL_VIDEO_OPENGL_PSGL */

/* vi: set ts=4 sw=4 expandtab: */
