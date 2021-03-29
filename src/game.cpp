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
#include "renderer.h"
#include "game.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

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
    GameState* game_state = (GameState*)memory->permanent_storage;


    Camera* cam = &game_state->renderer.camera;

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

inline internal LoadedBitmap
allocate_bitmap(GameMemory* memory, i32 w, i32 h)
{
    LoadedBitmap out;
    out.data = ALLOCATE_ARRAY((*memory), u8, (w*h));
    out.width = w;
    out.height = h;

    return out;
}

internal void
draw_debug_slider(Renderer* ren, RenderCommands* render_commands, vec3 position)
{
    vec2 scale = {100.0f, 20.0f};
    vec4 color = {0.3f, 0.3f, 0.3f, 1.0f};
    render_commands->draw_rect(ren, position, scale, color);

    position.x += 2.0f;
    position.y += 2.0f;
    position.z -= 0.1f;
    scale.x -= 4.0f;
    scale.y -= 6.0f;
    color.z = 0.666f;

    render_commands->draw_rect(ren, position, scale, color);
}

internal void
draw_debug_menu(Renderer* ren, RenderCommands* render_commands, char* name)
{
    vec4 menu_color = {0.8f, 0.8f, 0.8f, 1.0f};
    char* menu_items[] =
    {
        "fps",
        "char info",
        "number",
    };
    f32 width = 300.0f, height = 200.0f;

    f32 x = 0.0f;
    f32 y = 0.0f;
    f32 z = 0.0f;
    f32 in = 10.0f;

    render_commands->draw_rect(ren, {x, y, z}, {width, height}, menu_color);
    z -= 0.1f;

    for (sizet i = 0; i < ARRAY_COUNT(menu_items); ++i)
    {
        render_commands->draw_text(ren,
                                  menu_items[i], 
                                  {x + in, height - y - in, z},
                                  {1.0f, 1.0f, 1.0f, 1.0f});
        y += ren->font_size;
    }
}

extern "C" PLATFORM_API void
game_render(GameMemory* memory, RenderCommands* render_commands)
{
    GameState* game_state = (GameState*)memory->permanent_storage;

    if (!memory->is_initialized)
    {

        game_state->tone_hz = 256;
        game_state->tone_volume = 3000;
        game_state->t_sine = 0.0f;

        Renderer* ren = &game_state->renderer;

        DebugFileResult font_file = memory->DEBUG_read_entire_file("../consola.ttf");
        ren->font_bitmap = allocate_bitmap(memory, 512, 512);

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
            GAME_DEBUG_PRINT(memory, "Failed to bake font map\n");
        }

        render_commands->renderer_init(&game_state->renderer);

        memory->DEBUG_free_file_memory(font_file.data);
        memory->is_initialized = true;
    }
    else
    {
        Renderer* ren = &game_state->renderer;
        game_state->renderer.screen_width = memory->screen_width;
        game_state->renderer.screen_height = memory->screen_height;

        render_commands->renderer_begin(ren);
 
        draw_debug_menu(ren, render_commands, "Debug Menu");
        /*
        memory->draw_rectangle(&memory->renderer,
        {400.0f, 400.0f}, {200.0f, 300.0f}, {1.0f, 1.0f, 1.0f, 1.0f});
        memory->draw_rectangle(&memory->renderer,
        {100.0f, 400.0f}, {100.0f, 100.0f}, {1.0f, 0.0f, 0.0f, 1.0f});
        ren->draw_rectangle(ren, {100.0f, 100.0f}, {200.0f, 200.0f}, {1.0f, 1.0f, 0.0f, 1.0f});
        */
        render_commands->draw_cube(ren, {0.0f, 0.0f, 0.0f}, {100.0f, 100.0f, 100.0f}, {0.8f, 0.3f, 0.0f, 1.0f});
        render_commands->draw_text(ren, "Hello", {0.0f, (f32)ren->screen_height}, {1.0f, 0.0f, 0.5f, 1.0f});

        render_commands->renderer_end(ren);
    }
}

