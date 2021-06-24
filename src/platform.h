#if !defined(PLATFORM_H)
#define PLATFORM_H

struct FileResult
{
    void* data;
    i32 size;
};

typedef FileResult ReadEntireFileProc(char* path);
typedef void FreeEntireFileProc(void* memory);
typedef b8 WriteEntireFileProc(char* file_name, u32 size, void* memory);
typedef SpriteHandle LoadSpriteProc(Assets* assets, char* sprite_path);

typedef void RenderProc(Renderer* ren);


struct Platform
{
    ReadEntireFileProc* read_entire_file;
    FreeEntireFileProc* free_file_memory;
    WriteEntireFileProc* write_entire_file;

    RenderProc* init_renderer;
    RenderProc* end_frame;
};
#endif
