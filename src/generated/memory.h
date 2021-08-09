typedef struct MemoryArena MemoryArena;
struct MemoryArena
{
    sizet used;
    sizet size;
    u8 *base;
    u32 temp_arena_count;
};
typedef struct TemporaryArena TemporaryArena;
struct TemporaryArena
{
    MemoryArena *arena;
    sizet used;
};
