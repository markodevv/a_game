#if !defined(MAIN_H)
/* ========================================================================
   $File: $
   $Date: $
   $Creator: Marko Bisevac $
   $Notice: (C) Copyright 2021. All Rights Reserved. $
   ======================================================================== */


#ifdef GAME_DEBUG

#define ASSERT(condition, message) \
    if (!(condition)) { \
        printf(message); \
        printf("\n"); \
        assert(condition); \
    } 
#else

#define ASSERT(condifiton, message) 

#endif  

#define KYLOBYTES(n) n*1024
#define MEGABYTES(n) n*1024*1024
#define GIGABYTES(n) n*1024*1024*1024


struct GameMemory
{
    void* permanent_storage;
    sizet permanent_storage_size;

    void* temporary_storage;
    sizet temporary_storage_size;
};

struct GameState
{
};


#ifdef GAME_DEBUG

struct DebugFileResult
{
    void* data;
    sizet size;
};

internal DebugFileResult DEBUG_read_entire_file(char* path);
internal void DEBUG_free_file_memory(void* memory);
internal b8 DEBUG_write_entire_file(char* file_name, sizet size, void* memory);

#endif

internal void game_update_and_render(GameMemory* memory);

#define MAIN_H
#endif
