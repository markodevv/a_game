#if !defined(PLATFORM_H)
#define PLATFORM_H

struct FileResult
{
    void* data;
    i32 size;
};

struct Renderer;

typedef FileResult ReadEntireFileProc(char* path);
typedef void FreeEntireFileProc(void* memory);
typedef b8 WriteEntireFileProc(char* file_name, u32 size, void* memory);

typedef void RenderProc(Renderer* ren);

typedef void* Allocate(sizet size);
typedef void* Reallocate(void* ptr, sizet size);
typedef void Free(void* ptr);

struct Platform
{
    ReadEntireFileProc* ReadEntireFile;
    FreeEntireFileProc* FreeFileMemory;
    WriteEntireFileProc* WriteEntireFile;
    
    RenderProc* InitRenderer;
    RenderProc* RendererEndFrame;
    
    Allocate* Allocate;
    Reallocate* Reallocate;
    Free* Free;
#ifdef GAME_DEBUG
    typedef i32 LogFunc(const char* text, ...);
    LogFunc* LogM;
#endif
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

#define Allocate(type, count) (type*)g_Platform->Allocate(sizeof(type) * (count));
#define Reallocate(ptr, size) g_Platform->Reallocate(ptr, size);
#define Free(ptr) g_Platform->Free(ptr);

#endif


