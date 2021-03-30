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


extern "C" PLATFORM_API void
game_update(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input)
{
    GameState* game_state = (GameState*)memory->permanent_storage;


    Camera* cam = &game_state->renderer.camera;

    if (input->mouse.left_button.is_down)
    {
        DEBUG_PRINT("MouseX %f MouseY %f\n",
             input->mouse.position.x,
             input->mouse.position.y);

        DEBUG_PRINT("ScreenW %i ScreenH %i\n",
             memory->screen_width,
             memory->screen_height);
    }

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
    if (input->mouse.wheel_delta)
    {
        DEBUG_PRINT("Mouse scrolled\n");
        f32 scroll_amount = (f32)input->mouse.wheel_delta;
        cam->position.z -= scroll_amount;
    }
}

inline internal LoadedBitmap
allocate_bitmap(MemoryArena* memory, i32 w, i32 h)
{
    LoadedBitmap out;
    out.data = ALLOCATE(memory, u8, (w*h));
    out.width = w;
    out.height = h;

    return out;
}

global_variable vec4 ui_color = {0.1f, 0.1f, 0.1f, 1.0f};
global_variable vec4 hover_color = {0.7f, 0.7f, 0.7f, 1.0f};
global_variable vec4 clicked_color = {1.0f, 1.0f, 1.0f, 1.0f};

internal void
draw_debug_slider(DebugState* debug, GameInput* input, Renderer* ren, RenderCommands* render_commands, vec3 start_pos, f32 min, f32 max, f32* value, char* var_name)
{
    vec2 button_scale = {10.0f, 10.0f};
    vec2 scale = {100.0f, 10.0f};
    vec3 position = start_pos;

    render_commands->draw_rect(ren, position, {scale.x+button_scale.x, scale.y}, ui_color);
    vec3 text_pos = {(start_pos.x+scale.x+button_scale.x), start_pos.y, 0.0f};

    // TODO: make it use our temporary allocation;
    char *text = (char*)malloc(30);
    sprintf_s(text, 30, "%.2f", *value);
    render_commands->draw_text(ren, text, text_pos, {1.0f, 1.0f, 1.0f, 1.0f});
    free(text);


    vec4 color = {0.5f, 0.5f, 0.5f, 0.5f};

    if (debug->active_item.id == value)
    {
        if (button_released(input->mouse.left_button))
        {
            debug->active_item = {};
        }
        color = clicked_color;
        f32 slider_x = start_pos.x + (input->mouse.position.x - start_pos.x);
        slider_x = CLAMP(slider_x,
                         start_pos.x,
                         start_pos.x + scale.x);
        f32 update_value = ((slider_x - start_pos.x)/(scale.x));
        if (update_value)
        {
            *value = max * update_value;
        }
        else
        {
            *value = min;
        }
    }
    // else if hot
    else if (debug->hot_item.id == value)
    {
        if (button_pressed(input->mouse.left_button))
        {
            debug->active_item.id = value;
        }
        color = hover_color;
        debug->hot_item = {};
    }

    position.x += ((*value)/max) * scale.x;

    render_commands->draw_rect(ren, position, button_scale, color);

    if (point_is_inside(input->mouse.position, position.xy, button_scale))
    {
        debug->hot_item.id = value;
    }
}


internal void
draw_debug_fps(Renderer* ren, RenderCommands* render_commands, DebugState* debug)
{
    char* fps_text = "%.2f fps";
    char out[32];
    sprintf_s(out, fps_text, debug->game_fps);
    vec3 position = {0.0f,
                     (f32)ren->screen_height - ren->font_size,
                     0.0f};

    render_commands->draw_text(ren, out, position, {1.0f, 1.0f, 1.0f, 1.0f});
}

extern "C" PLATFORM_API void
game_render(GameMemory* memory, RenderCommands* render_commands, GameInput* input)
{
    GameState* game_state = (GameState*)memory->permanent_storage;

    if (!memory->is_initialized)
    {
        init_arena(&game_state->arena, 
                   (memory->permanent_storage_size - sizeof(GameState)),
                   (u8*)memory->permanent_storage + sizeof(GameState));

        memory->debug = ALLOCATE(&game_state->arena, DebugState);
        sub_arena(&game_state->arena, &memory->debug->arena, MEGABYTES(20));

        game_state->tone_hz = 256;
        game_state->tone_volume = 3000;
        game_state->t_sine = 0.0f;

        Renderer* ren = &game_state->renderer;

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

        render_commands->renderer_init(&game_state->renderer);

        memory->DEBUG_free_file_memory(font_file.data);
        memory->is_initialized = true;
    }
    else
    {
        Renderer* ren = &game_state->renderer;
        // NOTE: flip mouse y axis
        input->mouse.position.y = ren->screen_height - input->mouse.position.y;
        game_state->renderer.screen_width = memory->screen_width;
        game_state->renderer.screen_height = memory->screen_height;


        render_commands->renderer_begin(ren);
 
        local_persist f32 var = 0.0f;
        draw_debug_slider(memory->debug, input, ren, render_commands, {400, 400}, 0.0f, 30.0f, &var, "my variable");
        local_persist f32 other_var = 0.0f;
        draw_debug_slider(memory->debug,
                          input,
                          ren,
                          render_commands,
                          {400, 340},
                          0.0f, 0.5f, 
                          &ren->light_speed,
                          "light speed");
        draw_debug_fps(ren, render_commands, memory->debug);
        f32 random_value = 10.0f;
        render_commands->draw_cube(ren, {0.0f, 0.0f, 0.0f}, {100.0f, 100.0f, 100.0f}, {0.8f, 0.3f, 0.0f, 1.0f});
        render_commands->draw_rect(ren, {}, {50, 50}, {1.0f, 1.0f, 1.0f, 1.0f});

        render_commands->renderer_end(ren);
    }
}

