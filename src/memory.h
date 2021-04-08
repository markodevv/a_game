#if !defined(MEMORY_H)
#define MEMORY_H

#define PushMemory(arena, type, ...)  (type *)push_memory(arena, sizeof(type), __VA_ARGS__)

struct MemoryArena
{
    sizet used;
    sizet size;
    u8* base;
};

struct TemporaryArena
{
    MemoryArena* arena;
    sizet used;
};

inline void*
push_memory(MemoryArena* arena, sizet type_size, sizet count = 1)
{
    ASSERT((arena->size - arena->used) > (type_size * count));
    u8* out = arena->base + arena->used;
    arena->used += type_size * count;

    return out;
}

inline void
init_arena(MemoryArena* arena, sizet size, void* base)
{
    arena->used = 0;
    arena->size = size;
    arena->base = (u8*)base;
}

inline void
sub_arena(MemoryArena* main, MemoryArena* sub, sizet size)
{
    u8* base = PushMemory(main, u8, size);
    init_arena(sub, size, base);
}

internal TemporaryArena
begin_temporary_memory(MemoryArena* arena)
{
    TemporaryArena out = {};
    out.arena = arena;
    out.used = arena->used;

    return out;
}

internal void
end_temporary_memory(TemporaryArena* temp_arena)
{
    MemoryArena *arena = temp_arena->arena;
    ASSERT(arena->used >= temp_arena->used);
    arena->used = temp_arena->used;
}

#endif
