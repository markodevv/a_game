#ifdef GAME_DEBUG

#define DEBUG_FRAME_START(debug, input, assets, renderer) \
DebugFrameStart(debug, input, assets, renderer) 
#define DEBUG_FRAME_END() \
DebugFrameEnd()

#ifdef GAME_PROFILE

#define PROFILE_FUNCTION() \
Timer timer(__FUNCTION__)

#define PROFILE_SCOPE(name) \
Timer timer(name)
#else

#define PROFILE_FUNCTION()
#define PROFILE_SCOPE(name) 

#endif

#else

#define DEBUG_FRAME_START()
#define DEBUG_FRAME_END()

#endif


internal void
AddProfileEntry(char* name, f64 elapsed_sec)
{
    ProfileEntry* entry = PushMemory(&global_debug_state->arena, ProfileEntry);
    
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
        start_counter = global_debug_state->GetPrefCounter();
        name = _name;
    }
    
    ~Timer()
    {
        f64 elapsed_time = global_debug_state->GetElapsedSeconds(start_counter);
        AddProfileEntry(name, elapsed_time);
    }
};



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
DebugFrameStart(DebugState* debug, GameInput* input, Assets* assets, Renderer2D* ren)
{
    
    if (debug->temp_memory.arena)
    {
        DevUiStart(&debug->dev_ui, input, assets, ren);
        EndTemporaryMemory(&debug->temp_memory);
        if (global_is_edit_mode)
        {
            UiProfilerWindow(debug->profile_entries, &debug->dev_ui);
        }
    }
    
    debug->temp_memory = BeginTemporaryMemory(&debug->arena);
    
#if GAME_PROFILE
    /*
        LogM("-----FRAME DATA-----\n");
        LogM("FPS %f\n", debug->game_fps);
        LogM("Frame Time: %.2fms\n", elapsed_time * 1000.0f);
        PrintProfileData();
        LogM("---------------------\n");
    */
    f64 elapsed_time = global_debug_state->GetElapsedSeconds(debug->frame_start_cycles);
    debug->profile_entries = 0;
    debug->frame_start_cycles = global_debug_state->GetPrefCounter();
    debug->game_fps = 1.0f/elapsed_time;
#endif
    
}

internal void
DebugFrameEnd()
{
}