/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>

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

#ifndef _SDL_psl1ght_opengles_h
#define _SDL_psl1ght_opengles_h

#if SDL_VIDEO_OPENGL_EGL

#include "../SDL_egl_c.h"

typedef struct SDL_GLDriverData
{
    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
} SDL_GLDriverData;

/* OpenGLES functions */
#define PSL1GHT_GLES_GetAttribute _this->gl_data.egl_data->eglGetAttrib
#define PSL1GHT_GLES_GetProcAddress _this->gl_data.egl_data->eglGetProcAddress
#define PSL1GHT_GLES_UnloadLibrary _this->gl_data.egl_data->egl_unload
#define PSL1GHT_GLES_SetSwapInterval _this->gl_data.egl_data->eglSetSwapInterval
#define PSL1GHT_GLES_GetSwapInterval _this->gl_data.egl_data->eglGetSwapInterval
#define PSL1GHT_GLES_DeleteContext _this->gl_data.egl_data->eglDestroyContext

extern int PSL1GHT_GLES_LoadLibrary(_THIS, const char *path);
extern int PSL1GHT_GLES_SwapWindow(_THIS, SDL_Window * window);
extern int PSL1GHT_GLES_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context);
extern SDL_GLContext PSL1GHT_GLES_CreateContext(_THIS, SDL_Window * window);

#endif /* SDL_VIDEO_OPENGL_EGL */

#endif /* _SDL_psl1ght_opengles_h */

/* vi: set ts=4 sw=4 expandtab: */
