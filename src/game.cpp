#include <math.h>
#include <stdio.h>

#include "game.h"


internal void
game_play_sound(GameSoundBuffer* game_sound, GameState* game_state)
{
    i32 wave_period = game_sound->samples_per_sec / game_state->tone_hz;

    i16* samples = game_sound->samples;
    for (sizet sample_index = 0;
        sample_index < game_sound->sample_count;
        ++sample_index)
    {
        f32 sine_value = sinf(game_state->t_sine);
        i16 sample_value = (i16)(sine_value * game_state->tone_volume);
        *samples = sample_value;
        samples++;
        *samples = sample_value;
        samples++;
        game_state->t_sine += 2.0f * PI * (1.0f / (f32)wave_period);
        if (game_state->t_sine > 2.0f * PI)
        {
            game_state->t_sine -= 2.0f * PI;
        }
    }
}


extern "C" WIN32_DECLSPEC void
game_update(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input)
{
    GameState* game_state = (GameState*)memory;

    if (!memory->is_initialized)
    {
        game_state->tone_hz = 256;
        game_state->tone_volume = 3000;
        game_state->t_sine = 0.0f;
        memory->is_initialized = true;
    }

    if (input->move_left.pressed)
    {
        game_state->tone_volume += 300;
    }
    game_play_sound(game_sound, game_state);

}

extern "C" WIN32_DECLSPEC void
game_render()
{
}
