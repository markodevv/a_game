#define PushMemory(arena, type, ...)  (type *)_PushMemory(arena, sizeof(type), ##__VA_ARGS__)

internal void
MemoryClear(void* memory, sizet size)
{
    u8* next_to_clear = (u8*)memory;
    
    for (u32 i = 0; i < size; ++i)
    {
        *next_to_clear = 0;
        next_to_clear++;
    }
}


internal void*
_PushMemory(MemoryArena* arena, sizet type_size, sizet count = 1)
{
    Assert((arena->size - arena->used) > (type_size * count));
    u8* out = arena->base + arena->used;
    arena->used += type_size * count;
    
    MemoryClear(out, type_size);
    
    return out;
}

internal void
InitArena(MemoryArena* arena, sizet size, void* base)
{
    arena->used = 0;
    arena->size = size;
    arena->base = (u8*)base;
}

internal void
SubArena(MemoryArena* main, MemoryArena* sub, sizet size)
{
    u8* base = PushMemory(main, u8, size);
    InitArena(sub, size, base);
}

internal TemporaryArena
BeginTemporaryMemory(MemoryArena* arena)
{
    Assert(arena);
    TemporaryArena out = {};
    out.arena = arena;
    out.used = arena->used;
    
    ++arena->temp_arena_count;
    
    return out;
}

internal void
EndTemporaryMemory(TemporaryArena* temp_arena)
{
    MemoryArena *arena = temp_arena->arena;
    Assert(arena->used >= temp_arena->used);
    
    if (arena->temp_arena_count == 1)
    {
        arena->used = temp_arena->used;
    }
    
    *temp_arena = {};
    
    --arena->temp_arena_count;
}

internal TaskQueue
CreateTaskQueue(MemoryArena* arena, u32 count)
{
    TaskQueue result = {};
    result.tasks = PushMemory(arena, MemoryTask, count);
    result.count = count;
    
    for (u32 task_index = 0; 
         task_index < count; 
         ++task_index)
    {
        MemoryTask* task = result.tasks + task_index;
        SubArena(arena, &task->arena, Megabytes(1));
        task->is_used = false;
    }
    
    return result;
}

internal MemoryTask*
BeginMemoryTask(TaskQueue* task_queue)
{
    MemoryTask* result = 0;
    
    for (u32 i = 0; i < task_queue->count; ++i)
    {
        MemoryTask* task = task_queue->tasks + i;
        if (!task->is_used)
        {
            task->temp_memory = BeginTemporaryMemory(&task->arena);
            task->is_used = true;
            result = task;
            break;
        }
    }
    
    return result;
}

internal void 
EndMemoryTask(MemoryTask* task)
{
    EndTemporaryMemory(&task->temp_memory);
    task->is_used = false;
}