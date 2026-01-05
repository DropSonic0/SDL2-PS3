/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2010 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "../../SDL_internal.h"

/* PSL1GHT SDL video driver implementation (for PS3). Based on Dummy driver.
 *
 * Initial work by Ryan C. Gordon (icculus@icculus.org). A good portion
 *  of this was cut-and-pasted from Stephane Peter's work in the AAlib
 *  SDL video driver.  Renamed to "DUMMY" by Sam Lantinga.
 */

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_PSL1GHTvideo.h"
#include "SDL_PSL1GHTevents_c.h"
#include "SDL_PSL1GHTmodes_c.h"

#if SDL_VIDEO_OPENGL_EGL
#include "SDL_opengl.h"
#endif

#include <malloc.h>
#include <assert.h>

#include <rsx/rsx.h>

#define PSL1GHTVID_DRIVER_NAME "psl1ght"

/* Initialization/Query functions */
static int PSL1GHT_VideoInit(_THIS);
static void PSL1GHT_VideoQuit(_THIS);

#if SDL_VIDEO_OPENGL_EGL
static int PSL1GHT_GL_LoadLibrary(_THIS, const char *path);
static void *PSL1GHT_GL_GetProcAddress(_THIS, const char *proc);
static void PSL1GHT_GL_UnloadLibrary(_THIS);
static SDL_GLContext PSL1GHT_GL_CreateContext(_THIS, SDL_Window * window);
static int PSL1GHT_GL_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context);
static int PSL1GHT_GL_SetSwapInterval(_THIS, int interval);
static int PSL1GHT_GL_GetSwapInterval(_THIS);
static void PSL1GHT_GL_SwapWindow(_THIS, SDL_Window * window);
static void PSL1GHT_GL_DeleteContext(_THIS, SDL_GLContext context);
#endif

/* PS3GUI init functions : */
static void initializeGPU(SDL_DeviceData * devdata);

/* PSL1GHT driver bootstrap functions */

static int
PSL1GHT_Available(void)
{
    return (1);
}

static void
PSL1GHT_DeleteDevice(SDL_VideoDevice * device)
{
    deprintf (1, "PSL1GHT_DeleteDevice( %p)\n", device);
    SDL_free(device);
}

int
PSL1GHT_VideoInit(_THIS)
{
    SDL_DeviceData *devdata = NULL;

    devdata = (SDL_DeviceData*) SDL_calloc(1, sizeof(SDL_DeviceData));
    if (devdata == NULL) { 
        /* memory allocation problem */  
        SDL_OutOfMemory();
        return -1;
    } 

    _this->driverdata = devdata;

    PSL1GHT_InitSysEvent(_this);

    initializeGPU(devdata);
    PSL1GHT_InitModes(_this);

    gcmSetFlipMode(GCM_FLIP_VSYNC); // Wait for VSYNC to flip

    /* We're done! */
    return 0;
}

void
PSL1GHT_VideoQuit(_THIS)
{
#if SDL_VIDEO_OPENGL_EGL
    PSL1GHT_GL_UnloadLibrary(_this);
#endif
    deprintf (1, "PSL1GHT_VideoQuit()\n");
    PSL1GHT_QuitModes(_this);
    PSL1GHT_QuitSysEvent(_this);
    SDL_free( _this->driverdata);

}

void initializeGPU( SDL_DeviceData * devdata)
{
    deprintf (1, "initializeGPU()\n");
   // Allocate a 1Mb buffer, alligned to a 1Mb boundary to be our shared IO memory with the RSX.
    void *host_addr = memalign(1024*1024, 1024*1024);
    assert(host_addr != NULL);

    // Initilise Reality, which sets up the command buffer and shared IO memory
    rsxInit(&devdata->_CommandBuffer, 0x10000, 1024*1024, host_addr);
    assert(devdata->_CommandBuffer != NULL);
}

int
PSL1GHT_CreateWindow(_THIS, SDL_Window * window)
{
    SDL_WindowData *wdata;
    SDL_DeviceData *devdata = _this->driverdata;

    /* Allocate window internal data */
    wdata = (SDL_WindowData *) SDL_calloc(1, sizeof(SDL_WindowData));
    if (wdata == NULL) {
        return SDL_OutOfMemory();
    }

    /* Setup driver data for this window */
    window->driverdata = wdata;

#if SDL_VIDEO_OPENGL_EGL
    if (window->flags & SDL_WINDOW_OPENGL) {
        wdata->egl_surface = eglCreateWindowSurface(devdata->egl_display, devdata->egl_config, (EGLNativeWindowType) NULL, NULL);
        if (wdata->egl_surface == EGL_NO_SURFACE) {
            SDL_free(wdata);
            window->driverdata = NULL;
            return SDL_SetError("Could not create EGL window surface");
        }
    }
#endif

    SDL_SetKeyboardFocus(window);

    /* Window has been successfully created */
    return 0;
}

int
PSL1GHT_CreateWindowFrom(_THIS, SDL_Window * window, const void *data)
{
    return SDL_Unsupported();
}

void
PSL1GHT_SetWindowTitle(_THIS, SDL_Window * window)
{
}
void
PSL1GHT_SetWindowIcon(_THIS, SDL_Window * window, SDL_Surface * icon)
{
}
void
PSL1GHT_SetWindowPosition(_THIS, SDL_Window * window)
{
}
void
PSL1GHT_SetWindowSize(_THIS, SDL_Window * window)
{
}
void
PSL1GHT_ShowWindow(_THIS, SDL_Window * window)
{
}
void
PSL1GHT_HideWindow(_THIS, SDL_Window * window)
{
}
void
PSL1GHT_RaiseWindow(_THIS, SDL_Window * window)
{
}
void
PSL1GHT_MaximizeWindow(_THIS, SDL_Window * window)
{
}
void
PSL1GHT_MinimizeWindow(_THIS, SDL_Window * window)
{
}
void
PSL1GHT_RestoreWindow(_THIS, SDL_Window * window)
{
}
void
PSL1GHT_SetWindowGrab(_THIS, SDL_Window * window, SDL_bool grabbed)
{

}
void
PSL1GHT_DestroyWindow(_THIS, SDL_Window * window)
{
    if (window->driverdata) {
#if SDL_VIDEO_OPENGL_EGL
        SDL_DeviceData *devdata = _this->driverdata;
        SDL_WindowData *wdata = window->driverdata;

        if (devdata->egl_display != EGL_NO_DISPLAY && wdata->egl_surface != EGL_NO_SURFACE) {
            eglDestroySurface(devdata->egl_display, wdata->egl_surface);
        }
#endif
        SDL_free(window->driverdata);
        window->driverdata = NULL;
    }
}

SDL_bool PSL1GHT_HasScreenKeyboardSupport(_THIS)
{
    return SDL_FALSE;
}
void PSL1GHT_ShowScreenKeyboard(_THIS, SDL_Window *window)
{
}
void PSL1GHT_HideScreenKeyboard(_THIS, SDL_Window *window)
{
}

SDL_bool PSL1GHT_IsScreenKeyboardShown(_THIS, SDL_Window *window)
{
    return SDL_FALSE;
}

static SDL_VideoDevice *
PSL1GHT_CreateDevice(int devindex)
{
    SDL_VideoDevice *device;
    deprintf (1, "PSL1GHT_CreateDevice( %16X)\n", devindex);

    /* Initialize all variables that we clean on shutdown */
    device = (SDL_VideoDevice *) SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (device) {
        SDL_memset(device, 0, (sizeof *device));
    }
    else {
        SDL_OutOfMemory();
        SDL_free(device);
        return (0);
    }

    /* Set the function pointers */
    device->VideoInit = PSL1GHT_VideoInit;
    device->VideoQuit = PSL1GHT_VideoQuit;
    device->GetDisplayModes = PSL1GHT_GetDisplayModes;
    device->SetDisplayMode = PSL1GHT_SetDisplayMode;
    device->CreateSDLWindow = PSL1GHT_CreateWindow;
    device->CreateSDLWindowFrom = PSL1GHT_CreateWindowFrom;
    device->SetWindowTitle = PSL1GHT_SetWindowTitle;
    device->SetWindowIcon = PSL1GHT_SetWindowIcon;
    device->SetWindowPosition = PSL1GHT_SetWindowPosition;
    device->SetWindowSize = PSL1GHT_SetWindowSize;
    device->ShowWindow = PSL1GHT_ShowWindow;
    device->HideWindow = PSL1GHT_HideWindow;
    device->RaiseWindow = PSL1GHT_RaiseWindow;
    device->MaximizeWindow = PSL1GHT_MaximizeWindow;
    device->MinimizeWindow = PSL1GHT_MinimizeWindow;
    device->RestoreWindow = PSL1GHT_RestoreWindow;
    device->SetWindowGrab = PSL1GHT_SetWindowGrab;
    device->DestroyWindow = PSL1GHT_DestroyWindow;
    device->HasScreenKeyboardSupport = PSL1GHT_HasScreenKeyboardSupport;
    device->ShowScreenKeyboard = PSL1GHT_ShowScreenKeyboard;
    device->HideScreenKeyboard = PSL1GHT_HideScreenKeyboard;
    device->IsScreenKeyboardShown = PSL1GHT_IsScreenKeyboardShown;

    device->PumpEvents = PSL1GHT_PumpEvents;

#if SDL_VIDEO_OPENGL_EGL
    device->GL_LoadLibrary = PSL1GHT_GL_LoadLibrary;
    device->GL_GetProcAddress = PSL1GHT_GL_GetProcAddress;
    device->GL_UnloadLibrary = PSL1GHT_GL_UnloadLibrary;
    device->GL_CreateContext = PSL1GHT_GL_CreateContext;
    device->GL_MakeCurrent = PSL1GHT_GL_MakeCurrent;
    device->GL_SetSwapInterval = PSL1GHT_GL_SetSwapInterval;
    device->GL_GetSwapInterval = PSL1GHT_GL_GetSwapInterval;
    device->GL_SwapWindow = PSL1GHT_GL_SwapWindow;
    device->GL_DeleteContext = PSL1GHT_GL_DeleteContext;
#endif

    device->free = PSL1GHT_DeleteDevice;

    return device;
}

VideoBootStrap PSL1GHT_bootstrap = {
    PSL1GHTVID_DRIVER_NAME, "SDL psl1ght video driver",
    PSL1GHT_Available, PSL1GHT_CreateDevice
};

#if SDL_VIDEO_OPENGL_EGL

static int
PSL1GHT_GL_LoadLibrary(_THIS, const char *path) {
    SDL_DeviceData *devdata = _this->driverdata;

    devdata->egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (devdata->egl_display == EGL_NO_DISPLAY) {
        return SDL_SetError("Could not get EGL display");
    }

    if (eglInitialize(devdata->egl_display, NULL, NULL) != EGL_TRUE) {
        return SDL_SetError("Could not initialize EGL");
    }

    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

    EGLint num_configs;
    if (eglChooseConfig(devdata->egl_display, attribs, &devdata->egl_config, 1, &num_configs) != EGL_TRUE) {
        return SDL_SetError("Could not choose EGL config");
    }

    if (num_configs == 0) {
        return SDL_SetError("No EGL configs found");
    }

    return 0;
}

static void
PSL1GHT_GL_UnloadLibrary(_THIS) {
    SDL_DeviceData *devdata = _this->driverdata;

    if (devdata->egl_display != EGL_NO_DISPLAY) {
        eglTerminate(devdata->egl_display);
        devdata->egl_display = EGL_NO_DISPLAY;
    }
}

static void *
PSL1GHT_GL_GetProcAddress(_THIS, const char *proc)
{
    return eglGetProcAddress(proc);
}

static SDL_GLContext
PSL1GHT_GL_CreateContext(_THIS, SDL_Window * window)
{
    SDL_DeviceData *devdata = _this->driverdata;

    EGLint attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLContext context = eglCreateContext(devdata->egl_display, devdata->egl_config, EGL_NO_CONTEXT, attribs);
    if (context == EGL_NO_CONTEXT) {
        SDL_SetError("Could not create EGL context");
        return NULL;
    }

    return context;
}

static int
PSL1GHT_GL_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context)
{
    SDL_DeviceData *devdata = _this->driverdata;
    EGLSurface surface;

    if (context) {
        SDL_WindowData *wdata = (SDL_WindowData *) window->driverdata;
        surface = wdata->egl_surface;
    } else {
        surface = EGL_NO_SURFACE;
    }

    if (eglMakeCurrent(devdata->egl_display, surface, surface, context) != EGL_TRUE) {
        return SDL_SetError("Could not make EGL context current");
    }

    return 0;
}

static int
PSL1GHT_GL_SetSwapInterval(_THIS, int interval)
{
    SDL_DeviceData *devdata = _this->driverdata;

    if (eglSwapInterval(devdata->egl_display, interval) != EGL_TRUE) {
        return SDL_SetError("Could not set EGL swap interval");
    }

    devdata->egl_swap_interval = interval;
    return 0;
}

static int
PSL1GHT_GL_GetSwapInterval(_THIS)
{
    SDL_DeviceData *devdata = _this->driverdata;

    return devdata->egl_swap_interval;
}

static void
PSL1GHT_GL_SwapWindow(_THIS, SDL_Window * window)
{
    SDL_DeviceData *devdata = _this->driverdata;
    SDL_WindowData *wdata = window->driverdata;

    eglSwapBuffers(devdata->egl_display, wdata->egl_surface);
}

static void
PSL1GHT_GL_DeleteContext(_THIS, SDL_GLContext context)
{
    SDL_DeviceData *devdata = _this->driverdata;

    if (devdata->egl_display != EGL_NO_DISPLAY) {
        eglDestroyContext(devdata->egl_display, context);
    }
}

#endif

/* vi: set ts=4 sw=4 expandtab: */
