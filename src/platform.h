#if !defined(PLATFORM_H)
#define PLATFORM_H
#define internal static 
#define global_variable static 
#define local_persist static

#ifdef GAME_DEBUG

#include <assert.h>

#ifdef PLATFORM_WIN32
#define Assert(condition) \
if (!(condition)) *(int *)0 = 0 
#elif PLATFORM_LINUX 
#define Assert(condition) \
assert(condition)
#endif

#else

#define Assert(condition)

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

typedef u16 EntityId;
typedef u32 Layer;

typedef u32 SpriteID;

struct GameState;
struct Array;
typedef void (*SystemFunc)(GameState* gs, Array* ent);

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Kylobytes(n) n*1024
#define Megabytes(n) n*1024*1024
#define GigaBytes(n) n*1024*1024*1024


#ifdef PLATFORM_WIN32

#ifdef GAME_FILE
#define PLATFORM_API __declspec(dllexport)
#else
#define PLATFORM_API __declspec(dllimport)
#endif

#elif PLATFORM_LINUX

#define PLATFORM_API __attribute__((visibility("default")))

#endif

struct FileResult
{
    b8 is_valid;
    void* data;
    i32 size;
};

struct Renderer2D;

typedef FileResult ReadEntireFileProc(char* path);
typedef void FreeEntireFileProc(void* memory);
typedef b8 WriteEntireFileProc(char* file_name, u32 size, void* memory);

typedef void RenderProc(Renderer2D* ren);

typedef void* Allocate(sizet size);
typedef void* Reallocate(void* ptr, sizet size);
typedef void Free(void* ptr);

typedef f64 GetElapsedSeconds(u64 start);
typedef u64 GetPrefCounter();


typedef void (WorkCallback)(void* data);

struct WorkQueue;

typedef void (PushWorkEntryProc)(WorkQueue* queue, WorkCallback* callback, void* data);
typedef void (WaitForWorkersProc)(WorkQueue* queue);

struct Platform
{
    ReadEntireFileProc* ReadEntireFile;
    FreeEntireFileProc* FreeFileMemory;
    WriteEntireFileProc* WriteEntireFile;
    
    RenderProc* InitRenderer;
    RenderProc* RendererEndFrame;
    
    GetElapsedSeconds* GetElapsedSeconds;
    GetPrefCounter* GetPrefCounter;
    
    Allocate* Allocate;
    Reallocate* Reallocate;
    Free* Free;
    
    typedef i32 LogFunc(const char* text, ...);
    LogFunc* LogFunction;
    
    WorkQueue* work_queue;
    PushWorkEntryProc* PushWorkEntry;
    WaitForWorkersProc* WaitForWorkers;
};

internal void*
DefaultAllocate(sizet size)
{
    void* result = calloc(1, size);
    Assert(result);
    
    return result;
}

internal void
DefaultFree(void* ptr)
{
    Assert(ptr);
    free(ptr);
}

internal void*
DefaultReallocate(void* ptr, sizet size)
{
    Assert(ptr);
    void* result = realloc(ptr, size);
    Assert(result);
    
    return result;
}

internal void
MemCopy(void* dest, const void* src, sizet n)
{
    for (sizet i = 0; i < n; ++i)
    {
        u8* byte_dest = ((u8*)dest + i);
        u8 byte_src = *((u8*)src + i);
        *byte_dest = byte_src;
    }
}


internal bool
MemCompare(const void *mem1, void *mem2, sizet n)
{
    for (sizet i = 0; i < n; ++i)
    {
        u8 byte1 = *((u8*)mem1 + i);
        u8 byte2 = *((u8*)mem2 + i);
        
        if (byte1 != byte2)
        {
            return false;
        }
    }
    
    return true;
}

internal void
MemClear(void* mem, sizet size)
{
    for (sizet i = 0; i < size; ++i)
    {
        *((u8*)mem) = 0;
    }
}

#ifdef GAME_FILE
extern Platform g_Platform;
#else
Platform g_Platform;
#endif


#define Allocate(type, count) (type*)g_Platform.Allocate(sizeof(type) * (count));
#define Reallocate(ptr, size) g_Platform.Reallocate(ptr, size);
#define Free(ptr) g_Platform.Free(ptr);

#endif


