#if !defined(LOG_H)
#define LOG_H

#ifdef GAME_DEBUG
#include <stdio.h>
#include <string.h>
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#ifdef GAME_FILE

#define LogM(msg, ...) \
g_Platform.LogFunction("[%s:%d] "msg, __FILENAME__, __LINE__, ##__VA_ARGS__)

#define LogS(msg, ...) \
g_Platform.LogFunction(msg,  ##__VA_ARGS__)

#else

#define LogM(msg, ...) \
printf("[%s:%d] "msg, __FILENAME__, __LINE__, ##__VA_ARGS__)

#define LogS(msg, ...) \
printf(msg,  ##__VA_ARGS__)


#endif

#else

#define LogM(msg, ...) 
#define LogS(msh, ...)

#endif

#endif