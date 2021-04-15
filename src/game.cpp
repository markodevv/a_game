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

#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/scene.h>          // Output data structure
#include <assimp/postprocess.h>    // Post processing flags

#include "log.h"
#include "math.h"
#include "memory.h"
#include "debug.h"
#include "common.h"
#include "renderer.h"
#include "renderer.cpp"
#include "game.h"
#include "render_group.cpp"
#include "debug_ui.cpp"


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
        game_state->t_sine += 2.0f * PI_F * (1.0f / (f32)wave_period);
        if (game_state->t_sine > 2.0f * PI_F)
        {
            game_state->t_sine -= 2.0f * PI_F;
        }
    }
}



inline internal Image
allocate_image(MemoryArena* memory, i32 w, i32 h)
{
    Image image;
    image.data = PushMemory(memory, u8, (w*h));
    image.width = w;
    image.height = h;

    return image;
}



extern "C" PLATFORM_API void
game_update(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input)
{
    GameState* game_state = (GameState*)memory->permanent_storage;
    Platform *platform = &memory->platform;

    if (!memory->is_initialized)
    {
        TranState* trans_state = &game_state->trans_state;

        init_arena(&game_state->arena, 
                   (memory->permanent_storage_size - sizeof(GameState)),
                   (u8*)memory->permanent_storage + sizeof(GameState));

        init_arena(&trans_state->arena, 
                   (memory->temporary_storage_size),
                   (u8*)memory->temporary_storage);

        game_state->renderer = PushMemory(&game_state->arena, Renderer);

        Renderer* ren = game_state->renderer;

        sub_arena(&trans_state->arena, &trans_state->assets.arena, MEGABYTES(64));

        ren->light.ambient = V3(1.0f);
        ren->light.diffuse = V3(1.0f);
        ren->light.specular = V3(1.0f);

        ren->light_pos = V3(200, 100, 0.0f);

        ren->camera = {{0.0f, 0.0f, 200.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}};

        //ren->model = memory->load_3D_model(&ren->arena, "../assets/backpack/backpack.obj");
        trans_state->assets.models[M_ID_DUMMY] = platform->load_3D_model(&trans_state->assets, "../assets/character/character.obj");
        trans_state->assets.models[M_ID_ZOMBIE0] = platform->load_3D_model(&trans_state->assets, "../assets/zombies/obj/Zed_1.obj");
        trans_state->assets.models[M_ID_ZOMBIE1] = platform->load_3D_model(&trans_state->assets, "../assets/zombies/obj/Zed_2.obj");

/*
        opengl_load_model_to_gpu(ren->shader_program_3D, &game_state->assets.zombie_0);
        opengl_load_model_to_gpu(ren->shader_program_3D, &game_state->assets.zombie_1);
        opengl_load_model_to_gpu(ren->shader_program_3D, &game_state->assets.cube);
        */


        memory->debug = PushMemory(&game_state->arena, DebugState);
        sub_arena(&game_state->arena, &memory->debug->arena, MEGABYTES(12));


        FileResult font_file = platform->read_entire_file("../consola.ttf");

        DebugState* debug = memory->debug;
        debug->font_image = allocate_image(&debug->arena, 512, 512);
        debug->font_image.channels = 1;
        debug->font_size = 14.0f;

        /* TODO:
        i32 result = stbtt_BakeFontBitmap((u8*)font_file.data, 
                             0, debug->font_size,
                             (u8*)debug->font_image.data, 
                             debug->font_image.width, 
                             debug->font_image.height,
                             32, NUM_ASCII, 
                             debug->char_metrics);
        debug->x_advance = (debug->char_metrics + 32)->xadvance;

        if (!result)
        {
            DEBUG_PRINT("Failed to bake font map\n");
        }
        */

        game_state->minotaur_image = platform->load_image(&trans_state->assets, "../assets/minotaur.png");

        platform->init_renderer(game_state->renderer);

        platform->free_file_memory(font_file.data);
        memory->is_initialized = true;

    }

    Camera* cam = &game_state->renderer->camera;

    if (button_down(input->move_left))
    {
        cam->position.x -= 5.0f;
    }
    if (button_down(input->move_right))
    {
        cam->position.x += 5.0f;
    }
    if (button_down(input->move_up))
    {
        cam->position.y += 5.0f;
    }
    if (button_down(input->move_down))
    {
        cam->position.y -= 5.0f;
    }
    if (input->mouse.wheel_delta)
    {
        f32 scroll_amount = (f32)input->mouse.wheel_delta;
        cam->position.z -= scroll_amount * delta_time * 15;
    }

    Renderer* ren = game_state->renderer;
    input->mouse.position.y = ren->screen_height - input->mouse.position.y;
    ren->screen_width = memory->screen_width;
    ren->screen_height = memory->screen_height;

    process_debug_ui_interactions(memory->debug, input);

}




extern "C" PLATFORM_API void
game_render(GameMemory* memory)
{
    GameState* game_state = (GameState*)memory->permanent_storage;
    Renderer* ren = game_state->renderer;
    Platform *platform = &memory->platform;
    TranState* tran_state = &game_state->trans_state;


    TemporaryArena flush_memory = begin_temporary_memory(&tran_state->arena);

    ren->vertices_2D = PushMemory(&tran_state->arena, VertexData2D, MAX_VERTICES);

    RenderGroup* render_group = allocate_render_group(&tran_state->arena, 
                                                      MEGABYTES(4),
                                                      ren, 
                                                      &tran_state->assets);
    init_render_group(render_group,
                      mat4_orthographic((f32)ren->screen_width,
                                        (f32)ren->screen_height));
/*
    opengl_draw_model(ren, ren->cube, ren->light_pos, V3(5, 5, 5));
    opengl_draw_model(ren, ren->zombie_0, V3(100, 100, 0), V3(20, 20, 20));
    opengl_draw_model(ren, ren->zombie_1, V3(50, 50, 0), V3(1, 1, 1));
    opengl_draw_model(ren, ren->cube, V3(0, 0, 0), V3(20, 20, 20));
    */

    push_rect(render_group, V2(100, 100), V2(200, 300), V4(0.7f, 0.3f, 0.1f, 1.0f));
    push_rect(render_group, V2(50, 50), V2(50, 50), V4(0.3f, 0.3f, 1.0f, 1.0f));

    push_textured_rect(render_group, game_state->minotaur_image, V2(200, 200), V2(1000, 1000), V4(1.0f));

#if 0
    local_persist f32 var = 0.0f;
    debug_menu_begin(ren, memory->debug, {100, 200}, {400, (f32)ren->screen_height}, "Main Menu");

    draw_debug_fps(ren, memory->debug);

    draw_debug_slider(ren, memory->debug, 0.0f, 30.0f, &var, "my variable"); 
    debug_menu_newline(ren, memory->debug);

    if (draw_debug_button(ren, memory->debug, "button 1"))
    {
        DEBUG_PRINT("button 1 clicked");
    }

    if (draw_debug_button(ren, memory->debug, "button 2"))
    {
        DEBUG_PRINT("button 2 clicked");
    }

    debug_menu_newline(ren, memory->debug);

    draw_debug_slider(ren, memory->debug, -1000.0f, 1000.0f, &ren->light_pos.z, "light z"); 


    debug_vec3_slider(ren, memory->debug, &ren->light.ambient, "light.ambient");
    debug_vec3_slider(ren, memory->debug, &ren->light.diffuse, "light.diffuse");
    debug_vec3_slider(ren, memory->debug, &ren->light.specular, "light.specular");


    f32 random_value = 10.0f;
#endif

    process_render_group(render_group);

    platform->end_frame(render_group);

    end_temporary_memory(&flush_memory);
}

