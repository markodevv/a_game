#if !defined(LOG_H)
#define LOG_H

#ifdef GAME_DEBUG
#include <stdio.h>

// NOTE: hashes are a fix for linux, don't know if it works for windows
#define Print(msg, ...) \
printf(msg"\n", ##__VA_ARGS__) 

#else

#define Print(msg, ...) 

#endif

#endif
