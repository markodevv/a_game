#ifdef GAME_DEBUG

#define PROFILER_FRAME_START() \
ProfilerStart()
#define PROFILE_FUNCTION() \
Timer timer(__FUNCTION__)

#define PROFILE_SCOPE(name) \
Timer timer(name)

#define PROFILER_FRAME_END() \
ProfilerEnd()

#else

#define PROFILER_FRAME_START()
#define PROFILER_FRAME_END()
#define PROFILE_FUNCTION()

#endif

internal void
PrintProfileData()
{
    for (ProfileEntry* entry = global_debug_state->profile_entries;
         entry;
         entry = entry->next)
    {
        f64 ms_time = (entry->elapsed_sec * 1000.0f);
        LogM("%s : %.2fms\n", entry->name, ms_time);
    }
}

internal void
ProfilerStart()
{
    
    global_debug_state->profile_entries = 0;
    global_debug_state->frame_start_cycles = g_Platform.GetPrefCounter();
    global_debug_state->temp_memory = BeginTemporaryMemory(&global_debug_state->temp_arena);
}

internal void
ProfilerEnd()
{
    f64 elapsed_time = g_Platform.GetElapsedSeconds(global_debug_state->frame_start_cycles);
    printf("FRAME DATA\n");
    printf("Frame Time: %.2fms\n", elapsed_time * 1000.0f);
    PrintProfileData();
    EndTemporaryMemory(&global_debug_state->temp_memory);
}


internal void
AddProfileEntry(char* name, f64 elapsed_sec)
{
    ProfileEntry* entry = PushMemory(&global_debug_state->temp_arena, ProfileEntry);
    
    entry->name = name;
    entry->elapsed_sec = elapsed_sec;
    
    entry->next = global_debug_state->profile_entries;
    global_debug_state->profile_entries = entry;
}

struct Timer
{
    u64 start_counter;
    char* name;
    
    Timer(char* _name)
    {
        start_counter = g_Platform.GetPrefCounter();
        name = _name;
    }
    
    ~Timer()
    {
        f64 elapsed_time = g_Platform.GetElapsedSeconds(start_counter);
        AddProfileEntry(name, elapsed_time);
    }
};

