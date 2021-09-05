#pragma once

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
typedef struct MemoryTask MemoryTask;
struct MemoryTask
{
    MemoryArena arena;
    TemporaryArena temp_memory;
    b8 is_used;
};
typedef struct TaskQueue TaskQueue;
struct TaskQueue
{
    MemoryTask *tasks;
    u32 count;
};
