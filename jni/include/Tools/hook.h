#pragma once

#include <jni.h>

extern TempData *tempRead;
extern CanSeeData CanSee;
extern char* g_game_name;
extern uintptr_t libUE4;
extern uintptr_t Arrayaddr;
extern uintptr_t Matrix;
extern int Count;

void hack_thread();
