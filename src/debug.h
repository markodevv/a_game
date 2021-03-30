#if !defined(DEBUG_H)
#define DEBUG_H

#define FBLACK      "\033[30;"
#define FRED        "\033[31;"
#define FGREEN      "\033[32;"
#define FYELLOW     "\033[33;"
#define FBLUE       "\033[34;"
#define FPURPLE     "\033[35;"
#define D_FGREEN    "\033[6;"
#define FWHITE      "\033[7;"
#define FCYAN       "\x1b[36m"
#define FNONE        "\033[0m"
#define Cyan "\033[0;36m"

#ifdef GAME_DEBUG
#include <stdio.h>

#define ASSERT(condition) \
    if (!(condition)) *(int *)0 = 0 


#define DEBUG_PRINT(msg, ...) \
printf(msg, __VA_ARGS__) 

#define SUCCESS_PRINT() \
printf("SUCCESS\n")

#else

#define ASSERT(condition)
#define DEBUG_PRINT(msg, ...) 


#endif
#endif
