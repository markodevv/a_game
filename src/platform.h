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
typedef void MemCopy(void *dest, void *src, sizet n);


struct Platform
{
    ReadEntireFileProc* ReadEntireFile;
    FreeEntireFileProc* FreeFileMemory;
    WriteEntireFileProc* WriteEntireFile;

    RenderProc* InitRenderer;
    RenderProc* EndFrame;

    Allocate* Allocate;
    Reallocate* Reallocate;
    Free* Free;
    MemCopy* MemCopy;
};

internal void*
DefaultAllocate(sizet size)
{
    void* result = calloc(1, size);

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

    return result;
}

internal void
DefaultMemCopy(void *dest, void *src, sizet n)
{
    memcpy(dest, src, n);
}

#define Allocate(size) g_Platform->Allocate(size);
#define Reallocate(ptr, size) g_Platform->Reallocate(ptr, size);
#define Free(ptr) g_Platform->Free(ptr);
#define MemCopy(dest, src, size) g_Platform->MemCopy(dest, src, size);

#endif
