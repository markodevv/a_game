#include <math.h>
#include <stdio.h>

#define internal static 
#define global_variable static 
#define local_persist static

#include <stdint.h>
#include <stddef.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef i8 b8;
typedef size_t sizet;

#include "math.h"
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


extern "C" PLATFORM_API void
game_update(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input)
{
    GameState* game_state = (GameState*)memory;

    if (!memory->is_initialized)
    {
        memory->renderer_init(&memory->renderer);

        game_state->tone_hz = 256;
        game_state->tone_volume = 3000;
        game_state->t_sine = 0.0f;

        memory->is_initialized = true;
    }

    Camera* cam = &memory->renderer.camera;

    if (input->move_left.is_down)
    {
        cam->position.x -= 1.0f;
    }
    if (input->move_right.is_down)
    {
        cam->position.x += 1.0f;
    }
    if (input->move_up.is_down)
    {
        cam->position.y += 1.0f;
    }
    if (input->move_down.is_down)
    {
        cam->position.y -= 1.0f;
    }
    if (input->mouse_scroll.scrolled)
    {
        GAME_DEBUG_PRINT(memory, "Mouse scrolled\n");
        f32 scroll_amount = (f32)input->mouse_scroll.wheel_delta;
        cam->position.z -= scroll_amount;
    }
}

extern "C" PLATFORM_API void
game_render(GameMemory* memory)
{
    if (memory->is_initialized)
    {
        /*
        memory->draw_rectangle(&memory->renderer,
        {400.0f, 400.0f}, {200.0f, 300.0f}, {1.0f, 1.0f, 1.0f, 1.0f});
        memory->draw_rectangle(&memory->renderer,
        {100.0f, 400.0f}, {100.0f, 100.0f}, {1.0f, 0.0f, 0.0f, 1.0f});
        memory->draw_rectangle(&memory->renderer,
        {500.0f, 700.0f}, {100.0f, 400.0f}, {1.0f, 1.0f, 0.0f, 1.0f});
        */
        memory->draw_cube(&memory->renderer,
        {0.0f, 0.0f, 0.0f}, {100.0f, 100.0f, 100.0f}, {0.8f, 0.3f, 0.0f, 1.0f});

        memory->frame_end(&memory->renderer);
    }
}

