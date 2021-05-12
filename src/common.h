#if !defined(COMMON_H)
#define COMMON_H

#define internal static 
#define global_variable static 
#define local_persist static

#ifdef GAME_DEBUG

#define ASSERT(condition) \
    if (!(condition)) *(int *)0 = 0 

#else

#define ASSERT(condition)

#endif

#include <stdint.h>
#include <stddef.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef i8 b8;
typedef size_t sizet;

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Kylobytes(n) n*1024
#define Megabytes(n) n*1024*1024
#define GigaBytes(n) n*1024*1024*1024


#ifdef LIBRARY_IMPORT
#define PLATFORM_API __declspec(dllimport)
#else
#define PLATFORM_API __declspec(dllexport)
#endif



#endif 
