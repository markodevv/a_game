#define PushMemory(arena, type, ...)  (type *)push_memory(arena, sizeof(type), ##__VA_ARGS__)

internal void*
push_memory(MemoryArena* arena, sizet type_size, sizet count = 1)
{
    ASSERT((arena->size - arena->used) > (type_size * count));
    u8* out = arena->base + arena->used;
    arena->used += type_size * count;

    return out;
}

internal void
init_arena(MemoryArena* arena, sizet size, void* base)
{
    arena->used = 0;
    arena->size = size;
    arena->base = (u8*)base;
}

internal void
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

    ++arena->temp_arena_count;

    return out;
}

internal void
end_temporary_memory(TemporaryArena* temp_arena)
{
    MemoryArena *arena = temp_arena->arena;
    ASSERT(arena->used >= temp_arena->used);
    arena->used = temp_arena->used;

    *temp_arena = {};

    --arena->temp_arena_count;
}

internal void
memory_clear(void* memory, u32 size)
{
    u8* next_to_clear = (u8*)memory;

    for (u32 i = 0; i < size; ++i)
    {
        *next_to_clear = 0;
        next_to_clear++;
    }
}
