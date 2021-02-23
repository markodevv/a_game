#include "game.h"
#include "renderer.h"

internal void
game_play_sound(GameSoundBuffer* game_sound, i32 tone_hz, i32 tone_volume) 
{
    local_persist f32 t_sine = 0;
    i32 wave_period = game_sound->samples_per_sec / tone_hz;

    i16* samples = game_sound->samples;
    for (sizet sample_index = 0;
        sample_index < game_sound->sample_count;
        ++sample_index)
    {
        f32 sine_value = sinf(t_sine);
        i16 sample_value = (i16)(sine_value * tone_volume);
        *samples = sample_value;
        samples++;
        *samples = sample_value;
        samples++;
        t_sine += 2.0f * PI * (1.0f / (f32)wave_period);
    }
}



internal void
game_update_and_render(GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input)
{
    GameState* game_state = (GameState*)memory;

    if (!memory->is_initialized)
    {
        game_state->tone_hz = 512;
        game_state->tone_volume = 3000;
        memory->is_initialized = true;
    }

    if (input->move_left.pressed)
    {
        game_state->tone_hz -= 1;
    }

    if (input->move_right.pressed)
    {
        game_state->tone_hz += 1;
    }

    game_play_sound(game_sound, game_state->tone_hz, game_state->tone_volume);

    renderer_clear();
    renderer_draw();
}

