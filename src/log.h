#if !defined(LOG_H)
#define LOG_H

#ifdef GAME_DEBUG
#include <stdio.h>

// NOTE: hashes are a fix for linux, don't know if it works for windows
#ifdef PLATFORM_WIN32

internal char g_PrintBuffer[1024];

#ifndef WIN32_EXE_FILE

#define Print(msg, ...) \
sprintf(g_PrintBuffer, msg, ##__VA_ARGS__); \
g_Platform->Win32Print(g_PrintBuffer); \
g_Platform->Win32Print("\n");

#else

#define Print(msg, ...) \
sprintf(g_PrintBuffer, msg, ##__VA_ARGS__); \
OutputDebugString(g_PrintBuffer); \
OutputDebugString("\n");

#endif

#elif PLATFORM_LINUX

#define Print(msg, ...) \
printf(msg"\n", ##__VA_ARGS__) 

#endif

#else

#define Print(msg, ...) 

#endif

#endif
