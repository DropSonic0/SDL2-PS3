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

#if SDL_VIDEO_DRIVER_PSL1GHT && SDL_VIDEO_OPENGL_EGL

#include "SDL_psl1ghtvideo.h"
#include "SDL_psl1ghtopengles.h"
#include "SDL_psl1ghtevents_c.h"

/* EGL implementation of SDL OpenGLES support */

int
PSL1GHT_GLES_LoadLibrary(_THIS, const char *path) {
    return SDL_EGL_LoadLibrary(_this, path, (NativeDisplayType) 0, 0);
}

int
PSL1GHT_GLES_SwapWindow(_THIS, SDL_Window * window) {
    return SDL_EGL_SwapBuffers(_this, ((SDL_WindowData *) window->driverdata)->egl_surface);
}

int
PSL1GHT_GLES_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context) {
    return SDL_EGL_MakeCurrent(_this, ((SDL_WindowData *) window->driverdata)->egl_surface, context);
}

SDL_GLContext
PSL1GHT_GLES_CreateContext(_THIS, SDL_Window * window) {
    return SDL_EGL_CreateContext(_this, ((SDL_WindowData *) window->driverdata)->egl_surface);
}

#endif /* SDL_VIDEO_DRIVER_PSL1GHT && SDL_VIDEO_OPENGL_EGL */

/* vi: set ts=4 sw=4 expandtab: */
