#if !defined(LOG_H)
#define LOG_H

#ifdef GAME_DEBUG
#include <stdio.h>

#define PRINT(msg, ...) \
printf(msg"\n", __VA_ARGS__) 

#else

#define PRINT(msg, ...) 

#endif

#endif
