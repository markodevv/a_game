#if !defined(LOG_H)
#define LOG_H

#ifdef GAME_DEBUG
#include <stdio.h>

#define ASSERT(condition) \
    if (!(condition)) *(int *)0 = 0 


#define DEBUG_PRINT(msg, ...) \
printf(msg"\n", __VA_ARGS__) 

#define SUCCESS_PRINT() \
printf("SUCCESS\n")

#else

#define ASSERT(condition)
#define DEBUG_PRINT(msg, ...) 

#endif

#endif
