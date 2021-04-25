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

internal u32
add_entity(GameState* game_state)
{
    u32 id = game_state->num_entities++;
    ASSERT(game_state->num_entities < ArrayCount(game_state->entities));
    return id;
}

internal Entity*
get_entity(GameState* game_state, u32 entity_index)
{
    Entity* entity = 0;
    if (entity_index < game_state->num_entities && entity_index >= 0)
    {
        entity = &game_state->entities[entity_index];
    }

    return entity;
}




extern "C" PLATFORM_API void
game_update(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input)
{
    GameState* game_state = (GameState*)memory->permanent_storage;
    Platform *platform = &memory->platform;

    if (!memory->is_initialized)
    {
        memory->is_initialized = true;
        TranState* trans_state = &game_state->trans_state;

        init_arena(&game_state->arena, 
                   (memory->permanent_storage_size - sizeof(GameState)),
                   (u8*)memory->permanent_storage + sizeof(GameState));

        init_arena(&trans_state->arena, 
                   (memory->temporary_storage_size),
                   (u8*)memory->temporary_storage);

        game_state->renderer = PushMemory(&game_state->arena, Renderer);

        Renderer* ren = game_state->renderer;

        sub_arena(&trans_state->arena, &trans_state->assets.arena, Megabytes(64));

        ren->light.ambient = V3(1.0f);
        ren->light.diffuse = V3(1.0f);
        ren->light.specular = V3(1.0f);

        ren->light_pos = V3(200, 100, 0.0f);

        // NOTE: needs to be first image loaded
        ASSERT(trans_state->assets.num_images == 0);
        ren->white_image = platform->load_image(&trans_state->assets, "../assets/white.png");

        //ren->model = memory->load_3D_model(&ren->arena, "../assets/backpack/backpack.obj");
        trans_state->assets.models[M_ID_DUMMY] = platform->load_3D_model(&trans_state->assets, "../assets/character/character.obj");
        trans_state->assets.models[M_ID_ZOMBIE1] = platform->load_3D_model(&trans_state->assets, "../assets/zombies/obj/Zed_2.obj");


        memory->debug = PushMemory(&game_state->arena, DebugState);
        sub_arena(&game_state->arena, &memory->debug->arena, Megabytes(12));


        FileResult font_file = platform->read_entire_file("../consola.ttf");

        DebugState* debug = memory->debug;
        debug->font_image_handle = create_image_asset(&trans_state->assets, 512, 512, 1);
        debug->font_size = 14.0f;

        Image* font_image = get_loaded_image(&trans_state->assets, debug->font_image_handle);

        i32 result = stbtt_BakeFontBitmap((u8*)font_file.data, 
                             0, debug->font_size,
                             (u8*)font_image->data, 
                             font_image->width, 
                             font_image->height,
                             32, NUM_ASCII, 
                             debug->char_metrics);
        // NOTE: Advance X is the same for all characters
        // thats why we set this
        debug->x_advance = (debug->char_metrics + 32)->xadvance;

        if (!result)
        {
            DEBUG_PRINT("Failed to bake font map\n");
        }

        game_state->minotaur_image = platform->load_image(&trans_state->assets, "../assets/minotaur.png");

        platform->init_renderer(game_state->renderer);

        platform->free_file_memory(font_file.data);

        u32 player_entity_id = add_entity(game_state);
        Entity* player_ent = get_entity(game_state, player_entity_id);

        player_ent->transform.position = V2(100, 100);
        player_ent->render.color = V4(0.5f, 0.1f, 0.0f, 1.0f);
        player_ent->render.scale = V2(100, 100);

        u32 tile_map[10][19] = {
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        };

        for (u32 y = 0; y < 10; ++y)
        {
            for (u32 x = 0; x < 19; ++x)
            {
                game_state->tile_map[9-y][x] = tile_map[y][x];
            }
        }
    }


    Entity* player_ent= get_entity(game_state, game_state->player_entity_index);
    if (player_ent)
    {
        if (button_down(input->move_left))
        {
            player_ent->transform.position.x -= 5.0f;
        }
        if (button_down(input->move_right))
        {
            player_ent->transform.position.x += 5.0f;
        }

        if (player_ent->transform.position.y < 0.0f)
        {
            player_ent->transform.position.y = 0.0f;
        }
    }

    Camera* cam = &game_state->render_setup.camera;

    if (game_state->is_free_camera)
    {
        if (input->mouse.wheel_delta)
        {
            f32 scroll_amount = (f32)input->mouse.wheel_delta;
            cam->position.z -= scroll_amount;
            DEBUG_PRINT("cam Z %f", cam->position.z);
            DEBUG_PRINT("scroll  %f", scroll_amount);
        }

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

    if (game_state->is_free_camera)
    {
        game_state->render_setup.projection = mat4_perspective((f32)ren->screen_width, 
                                                               (f32)ren->screen_height,
                                                               90.0f,
                                                               1.0f,
                                                               100000.0f);
    }
    else
    {
        Entity* player = get_entity(game_state, game_state->player_entity_index);
        game_state->render_setup.camera.up = V3(0, 1, 0);
        game_state->render_setup.camera.direction = V3(0, 0, 1);
        game_state->render_setup.camera.position.x = player->transform.position.x;
        game_state->render_setup.camera.position.y = player->transform.position.y;
        game_state->render_setup.camera.position.z = 1.0f;
        game_state->render_setup.projection = mat4_orthographic((f32)ren->screen_width, 
                                                                (f32)ren->screen_height);
    }
    RenderGroup render_group = render_group_begin(&game_state->render_setup,
                                                      ren, 
                                                      &tran_state->assets);

    memory->debug->render_setup.projection = mat4_orthographic((f32)ren->screen_width,
                                                       (f32)ren->screen_height);
    memory->debug->render_setup.camera = {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}};
    memory->debug->render_group = render_group_begin(&memory->debug->render_setup,
                                                     ren, 
                                                     &tran_state->assets);

    for (u32 i = 0; i < game_state->num_entities; ++i)
    {
        Entity* entity = get_entity(game_state, i);
        push_quad(&render_group, 
                  entity->transform.position, 
                  entity->render.scale, 
                  entity->render.color);
    }

    for (u32 x = 0; x < 19; ++x)
    {
        for (u32 y = 0; y < 10; ++y)
        {
            if (game_state->tile_map[y][x])
            {
                push_quad(&render_group, 
                          V2((f32)x * 100, (f32)y * 100),
                          V2(95, 95),
                          V4(0.3f, 0.5f, 1.0f, 1.0f));
            }
        }
    }

    push_quad(&render_group, game_state->minotaur_image, V2(600, 200), V2(500, 500), V4(1.0f));



    local_persist f32 var = 0.0f;
    debug_menu_begin(memory->debug, {100, 200}, {400, (f32)ren->screen_height}, "Main Menu");

    debug_fps(memory->debug);

    debug_slider(memory->debug, 0.0f, 30.0f, &var, "my variable"); 
    debug_menu_newline(memory->debug);

    if (debug_button(memory->debug, "button 1"))
    {
        DEBUG_PRINT("button 1 clicked");
    }

    if (debug_button(memory->debug, "button 2"))
    {
        DEBUG_PRINT("button 2 clicked");
    }

    debug_menu_newline(memory->debug);

    debug_slider(memory->debug, -1000.0f, 1000.0f, &ren->light_pos.z, "light z"); 


    debug_vec3_slider(memory->debug, &ren->light.ambient, "light.ambient");
    debug_vec3_slider(memory->debug, &ren->light.diffuse, "light.diffuse");
    debug_vec3_slider(memory->debug, &ren->light.specular, "light.specular");

    debug_checkbox(memory->debug, &game_state->is_free_camera);



    platform->end_frame(ren);


    end_temporary_memory(&flush_memory);
}

