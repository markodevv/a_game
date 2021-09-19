#pragma once

typedef struct DebugState DebugState;
struct DebugState
{
    MemoryArena arena;
    TemporaryArena temp_memory;

    DevUI dev_ui;

    ProfileEntry *profile_entries;
    u64 frame_start_cycles;

    GetElapsedSecondsProc GetElapsedSeconds;
    GetPrefCounterProc GetPrefCounter;

    b32 have_read_config;

    f32 game_fps;
};
