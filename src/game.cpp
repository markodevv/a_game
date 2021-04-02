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

#include "log.h"
#include "math.h"
#include "memory.h"
#include "debug.h"
#include "renderer.h"
#include "game.h"
#include "renderer.cpp"
#include "debug_ui.cpp"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"


// NOTE: Debug data

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

inline internal LoadedBitmap
allocate_bitmap(MemoryArena* memory, i32 w, i32 h)
{
    LoadedBitmap out;
    out.data = PushMemory(memory, u8, (w*h));
    out.width = w;
    out.height = h;

    return out;
}

extern "C" PLATFORM_API void
game_update(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input)
{
    GameState* game_state = (GameState*)memory->permanent_storage;

    if (!memory->is_initialized)
    {
        init_arena(&game_state->arena, 
                   (memory->permanent_storage_size - sizeof(GameState)),
                   (u8*)memory->permanent_storage + sizeof(GameState));

        init_arena(&game_state->temporary_arena, 
                   (memory->temporary_storage_size - sizeof(GameState)),
                   (u8*)memory->temporary_storage);

        memory->debug = PushMemory(&game_state->arena, DebugState);
        sub_arena(&game_state->arena, &memory->debug->arena, MEGABYTES(20));

        game_state->renderer = PushMemory(&game_state->arena, Renderer);

        game_state->tone_hz = 256;
        game_state->tone_volume = 3000;
        game_state->t_sine = 0.0f;

        Renderer* ren = game_state->renderer;
        ren->renderer_init = memory->renderer_init;
        ren->renderer_begin = memory->renderer_begin;
        ren->renderer_end = memory->renderer_end;

        DebugFileResult font_file = memory->DEBUG_read_entire_file("../consola.ttf");

        ren->font_bitmap = allocate_bitmap(&game_state->arena, 512, 512);


        ren->font_size = 14.0f;
        i32 result = stbtt_BakeFontBitmap((u8*)font_file.data, 
                             0, ren->font_size,
                             ren->font_bitmap.data, 
                             ren->font_bitmap.width, 
                             ren->font_bitmap.height,
                             32, NUM_ASCII, 
                             ren->char_metrics);

        if (!result)
        {
            DEBUG_PRINT("Failed to bake font map\n");
        }

        ren->renderer_init(game_state->renderer);

        memory->DEBUG_free_file_memory(font_file.data);
        memory->is_initialized = true;
    }

    Camera* cam = &game_state->renderer->camera;

    if (button_down(input->move_left))
    {
        cam->position.x -= 1.0f;
    }
    if (button_down(input->move_right))
    {
        cam->position.x += 1.0f;
    }
    if (button_down(input->move_up))
    {
        cam->position.y += 1.0f;
    }
    if (button_down(input->move_down))
    {
        cam->position.y -= 1.0f;
    }
    if (input->mouse.wheel_delta)
    {
        f32 scroll_amount = (f32)input->mouse.wheel_delta;
        cam->position.z -= scroll_amount * delta_time * 10;
    }


    Renderer* ren = game_state->renderer;
    input->mouse.position.y = ren->screen_height - input->mouse.position.y;
    ren->screen_width = memory->screen_width;
    ren->screen_height = memory->screen_height;

    process_debug_ui_interactions(memory->debug, input);

    debug_end(memory->debug);
}





extern "C" PLATFORM_API void
game_render(GameMemory* memory)
{
        GameState* game_state = (GameState*)memory->permanent_storage;
        Renderer* ren = game_state->renderer;
        // NOTE: flip mouse y axis


        ren->renderer_begin(ren);
 
        debug_begin(memory->debug);

        local_persist f32 var = 0.0f;
        debug_menu_begin(ren, memory->debug, {100, 200}, {400, 300}, "Main Menu");

        draw_debug_slider(ren, memory->debug, 0.0f, 30.0f, &var, "my variable"); 
        debug_menu_newline(ren, memory->debug);
        draw_debug_slider(ren, memory->debug, 0.0f, 0.5f, &ren->light_speed, "light speed");

        if (draw_debug_button(ren, memory->debug, "button 1"))
        {
            DEBUG_PRINT("button 1 clicked");
        }
        if (draw_debug_button(ren, memory->debug, "button 2"))
        {
            DEBUG_PRINT("button 2 clicked");
        }

        draw_debug_fps(ren, memory->debug);


        f32 random_value = 10.0f;
        draw_cube(ren, {0.0f, 0.0f, 0.0f}, {100.0f, 100.0f, 100.0f}, {0.8f, 0.3f, 0.0f, 1.0f});
        draw_rect(ren, {}, {50, 50}, {1.0f, 1.0f, 1.0f, 1.0f});

        ren->renderer_end(ren);
}

