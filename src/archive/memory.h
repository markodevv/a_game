#if !defined(MEMORY_H)
#define MEMORY_H


struct MemoryArena
{
    sizet used;
    sizet size;
    u8* base;
    u32 temp_arena_count;
};

struct TemporaryArena
{
    MemoryArena* arena;
    sizet used;
};


#endif
