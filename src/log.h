#if !defined(LOG_H)
#define LOG_H

#ifdef GAME_DEBUG
#include <stdio.h>

#ifdef GAME_FILE

#define LogM(msg, ...) \
g_Platform->LogM(msg, ##__VA_ARGS__) 

#else

#define LogM(msg, ...) \
printf(msg, ##__VA_ARGS__) 

#endif

#else

#define LogM(msg, ...) 

#endif

#endif