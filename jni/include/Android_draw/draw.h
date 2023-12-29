#ifndef NATIVESURFACE_DRAW_H
#define NATIVESURFACE_DRAW_H

#include <iostream>
#include <thread>
#include <chrono>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <android/native_window.h>
#include "native_surface.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_android.h"
#include <EGL/eglext.h>
#include <GLES3/gl3platform.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl32.h>

using namespace std;
using namespace std::chrono_literals;

extern void *handle;// 动态库方案
extern EGLDisplay display;
extern EGLConfig config;
extern EGLSurface surface;
extern ANativeWindow *native_window;
extern ANativeWindow *(*createNativeWindow)(const char *surface_name ,uint32_t screen_width ,uint32_t screen_height,bool author);
extern EGLContext context;
struct Screen {
    float ScreenX;
    float ScreenY;
};


extern float NumIo[50];
extern Screen full_screen;
extern int Orientation;
extern int screen_x, screen_y;
extern int init_screen_x, init_screen_y;
extern bool g_Initialized;
string exec(string command);
int init_egl(int _screen_x,int _screen_y, bool log = false);
void screen_config();
void shutdown();
void tick();
void ImGui_init();
void DrawPlayer();
void AimBotAuto();
void GetTouch();
void *AimCalcu();
void DrawInit();
int DrawInt();

#endif
