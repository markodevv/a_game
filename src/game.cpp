// 090035212100108
#include <math.h>
#include <stdio.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>


#include "common.h"
#include "memory.h"
#include "string.cpp"
#include "log.h"
#include "math.h"
#include "input.h"
#include "debug.h"
#include "renderer.h"
#include "renderer.cpp"
#include "game.h"
#include "render_group.cpp"
#include "asset_loading.cpp"
#include "debug_ui.cpp"



// NOTE: Debug data

global_variable b8 global_is_edit_mode = 1;

internal void
game_play_sound(GameSoundBuffer* game_sound, GameState* game_state)
{
    i32 wave_period = game_sound->samples_per_sec / game_state->tone_hz;

    i16* samples = game_sound->samples;
    for (i32 sample_index = 0;
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


internal b8
is_colliding(vec2 p1, vec2 s1, vec2 p2, vec2 s2)
{
    return (p1.x < p2.x + s2.x &&
            p1.x + s1.x > p1.x &&
            p1.y < p2.y + s2.y &&
            p1.y + s1.y > s2.y);

}

internal void
add_force(Entity* entity, vec2 force)
{
    entity->rigidbody.acceleration += force;
}

// F = m*a

internal void
integrate(Entity* entity, f32 dt)
{
    if (entity->rigidbody.mass <= 0.0f)
        return;


    add_force(entity, V2(0, -1) * 300);

    entity->position += entity->rigidbody.velocity * dt;

    entity->rigidbody.acceleration /= entity->rigidbody.mass;

    entity->rigidbody.velocity += entity->rigidbody.acceleration * dt;

    f32 drag = powf(0.5f, dt);
    entity->rigidbody.velocity *= drag;

    entity->rigidbody.acceleration = V2(0.0f);
}


internal void
integrate(Particle* particle, f32 dt)
{
    if (particle->rigidbody.mass <= 0.0f)
        return;

    particle->position += particle->rigidbody.velocity * dt;
}

internal ParticleEmitter
create_particle_emitter(MemoryArena* arena,
                       vec2 min_vel,
                       vec2 max_vel,
                       u32 particle_spawn_rate,
                       Color color,
                       vec2 size,
                       u32 num_particles)
{
    ParticleEmitter result = {};

    result.min_vel = min_vel;
    result.max_vel = max_vel;

    result.particle_spawn_rate = particle_spawn_rate;

    result.render.size = size;
    result.render.color = color;
    
    result.max_particles = num_particles;

    result.particles = PushMemory(arena, Particle, num_particles);

    return result;
}

internal f32
random_between_two_floats(f32 min, f32 max)
{
    f32 random = ((f32)rand()) / (f32)RAND_MAX;
    f32 diff = max - min;
    f32 r = random * diff;
    return min + r;
}

internal vec2
random_between_two_vectors(vec2 min, vec2 max)
{
    return V2(random_between_two_floats(min.x, max.x),
              random_between_two_floats(min.y, max.y));
}


extern "C" PLATFORM_API void
game_main_loop(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input)
{
    GameState* game_state = (GameState*)memory->permanent_storage;
    Renderer* ren = &game_state->renderer;
    Platform *platform = &memory->platform;

    if (!memory->is_initialized)
    {
        memory->is_initialized = true;


        init_arena(&game_state->arena, 
                   (memory->permanent_storage_size - sizeof(GameState)),
                   (u8*)memory->permanent_storage + sizeof(GameState));

        init_arena(&game_state->transient_arena, 
                   (memory->temporary_storage_size),
                   (u8*)memory->temporary_storage);

        sub_arena(&game_state->transient_arena, &game_state->assets.arena, Megabytes(64));



        Renderer* ren = &game_state->renderer;



        ren->assets = &game_state->assets;

        // NOTE: needs to be first image loaded
        ASSERT(game_state->assets.num_sprites == 0);
        ren->white_sprite = load_sprite(platform, &game_state->assets, "../assets/white.png");


        memory->debug = PushMemory(&game_state->arena, DebugState);
        sub_arena(&game_state->arena, &memory->debug->arena, Megabytes(12));
#ifdef PLATFORM_WIN32
        memory->debug->font = ui_load_font(&memory->debug->arena,
                                              platform,
                                              &trans_state->assets,
                                              "../consola.ttf",
                                              16);
#elif PLATFORM_LINUX
        memory->debug->font = ui_load_font(&memory->debug->arena,
                                              platform,
                                              &game_state->assets,
                                              "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
                                              16);
#endif
        for (u32 i = 0; i < ArrayCount(memory->debug->window_items); ++i)
        {
            memory->debug->window_items[i].menu_is_active = true;
        }


        game_state->minotaur_sprite = load_sprite(platform, &game_state->assets, "../assets/minotaur.png");
        game_state->hero_sprite_sheet = load_sprite(platform, &game_state->assets, 
        "../assets/platform_metroidvania asset pack v1.01/herochar sprites(new)/herochar_spritesheet(new).png");
        game_state->hero_sprite = subsprite_from_spritesheet(&game_state->assets,
                                                                    game_state->hero_sprite_sheet,
                                                                    0, 11,
                                                                    16, 16);
        game_state->backgroud_sprite = load_sprite(platform, &game_state->assets,
                                                   "../assets/platform_metroidvania asset pack v1.01/tiles and background_foreground/background.png");

        platform->init_renderer(&game_state->renderer);




        game_state->particle_emitter = create_particle_emitter(&game_state->arena,
                                                             V2(-50, 50),
                                                             V2(80, 120),
                                                             4,
                                                             COLOR(50, 100, 22, 255),
                                                             V2(10, 10),
                                                             128);

        u32 player_entity_id = add_entity(game_state);
        Entity* player_ent = get_entity(game_state, player_entity_id);

        player_ent->position = V2(100, 300);
        player_ent->render.color = COLOR(255);
        player_ent->render.size = V2(60, 60);

        player_ent->rigidbody.mass = 1.0f;

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


        game_state->tile_size = 60;

        for (u32 y = 0; y < 10; ++y)
        {
            for (u32 x = 0; x < 19; ++x)
            {
                game_state->tile_map[9-y][x] = tile_map[y][x];
            }
        }
    }

    TemporaryArena main_temp_arena = begin_temporary_memory(&game_state->transient_arena);

    Camera main_cam = {};
    main_cam.up = V3(0, 1, 0);
    main_cam.direction = V3(0, 0, 1);
    main_cam.position.z = 1.0f;

    RenderGroup* render_group = setup_render_group(&game_state->transient_arena,
                                                   mat4_orthographic((f32)ren->screen_width,
                                                                    (f32)ren->screen_height),
                                                   main_cam,
                                                   ren, 
                                                   &game_state->assets);


    Entity* player_ent = get_entity(game_state, game_state->player_entity_index);

    if (player_ent)
    {
        if (button_down(input->move_left))
        {
            add_force(player_ent, V2(-1000.0f, 0.0f));
        }
        if (button_down(input->move_right))
        {
            add_force(player_ent, V2(1000.0f, 0.0f));
        }
        if (button_down(input->move_up))
        {
            add_force(player_ent, V2(0.0f, 1000.0f));
        }

        if (player_ent->position.y <= 100.0f)
        {
            add_force(player_ent, V2(0.0f, 3000.0f));
        }

        push_quad(render_group, 
                  &game_state->hero_sprite,
                  player_ent->position, 
                  player_ent->render.size, 
                  player_ent->render.color,
                  LAYER_MID);


        // f32 distance = vec2_distance(player_ent->position, V2(300, 0));
        // PRINT("Distance %.2f", distance);
        // if (distance <= 300.0f)
        // {
            // add_force(player_ent, V2(0.0f, 10000.0f));
        // }
// 
        integrate(player_ent, delta_time);
    }


    Camera* cam = &game_state->render_setup.camera;

    if (game_state->is_free_camera)
    {
        if (input->mouse.wheel_delta)
        {
            f32 scroll_amount = (f32)input->mouse.wheel_delta;
            cam->position.z -= scroll_amount;
            PRINT("cam Z %f", cam->position.z);
            PRINT("scroll  %f", scroll_amount);
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



    
    ParticleEmitter* particle_emitter = &game_state->particle_emitter;
    particle_emitter->position = player_ent->position;
    for (u32 particle_index = 0; 
         particle_index < particle_emitter->max_particles;
         ++particle_index)
    {
        Particle* particle = particle_emitter->particles + particle_index;
        integrate(particle, delta_time);

        push_quad(render_group, 
                  particle->position,
                  particle_emitter->render.size,
                  particle_emitter->render.color,
                  LAYER_MID);
    }

    for (u32 i = 0; i < particle_emitter->particle_spawn_rate; ++i)
    {
        if (particle_emitter->particle_index >= particle_emitter->max_particles)
        {
            particle_emitter->particle_index = 0;
        }

        Particle* particle = particle_emitter->particles + particle_emitter->particle_index;
        particle->position = particle_emitter->position;
        particle->rigidbody.velocity = random_between_two_vectors(particle_emitter->min_vel,
                                                                  particle_emitter->max_vel);
        particle->rigidbody.mass = 1;


        ++particle_emitter->particle_index;
    }

    input->mouse.position.y = ren->screen_height - input->mouse.position.y;
    ren->screen_width = memory->screen_width;
    ren->screen_height = memory->screen_height;




    push_quad(render_group,
              V2(0),
              V2((f32)ren->screen_width,
                 (f32)ren->screen_height),
              COLOR(255),
              LAYER_BACK,
              game_state->backgroud_sprite);




    for (u32 x = 0; x < 19; ++x)
    {
        for (u32 y = 0; y < 10; ++y)
        {
            if (game_state->tile_map[y][x])
            {
                push_quad(render_group, 
                          V2((f32)x * game_state->tile_size, (f32)y * game_state->tile_size),
                          V2(game_state->tile_size - 4.0f, game_state->tile_size - 4.0f),
                          COLOR(75, 75, 0, 255),
                          LAYER_MID);
            }
        }
    }



    if (global_is_edit_mode)
    {
        imediate_ui(memory->debug, input, &game_state->assets, ren);

        ui_fps(memory->debug);

        ui_window_begin(memory->debug, "Window");

        if (ui_submenu(memory->debug, ("Player")))
        {
            Entity* player = game_state->entities;

            UI_Float32Editbox(memory->debug, &player->position, "player position");
            UI_Float32Editbox(memory->debug, &player->rigidbody.acceleration, "player acceleration");
            UI_Float32Editbox(memory->debug, &player->rigidbody.velocity, "player velocity");
            UI_Float32Editbox(memory->debug, &player->rigidbody.mass, "player mass");

        }
        if (ui_submenu(memory->debug, ("Particle Emitter")))
        {
            ParticleEmitter* emitter = &game_state->particle_emitter;

            UI_Float32Editbox(memory->debug, &emitter->min_vel, "min velocity");
            UI_Float32Editbox(memory->debug, &emitter->max_vel, "max velocity");
            UI_Int32Editbox(memory->debug, &emitter->particle_spawn_rate, "spawn rate");
            UI_Float32Editbox(memory->debug, &emitter->render.size, "size");
            ui_color_picker(memory->debug, &emitter->render.color, "color mhehe");
        }

        ui_window_end(memory->debug);

        ui_window_begin(memory->debug, "Other Window 01", V2(0), V2(310, 200));

        ui_button(memory->debug, "Beten", V2(0, 500), V2(150, 50));
        ui_cursor_sameline(memory->debug);
        ui_button(memory->debug, "Beten", V2(0, 500), V2(150, 50));

        ui_window_end(memory->debug);

        ui_window_begin(memory->debug, "Other Window 02", V2(0), V2(300, 400));

        local_persist f32 test = 100.0f;
        ui_slider(memory->debug, 0, 1000, &test, "test var");
        ui_slider(memory->debug, 0, 1000, &test, "test var");
        ui_slider(memory->debug, 0, 1000, &test, "test var");

        ui_window_end(memory->debug);

        end_temporary_memory(&memory->debug->temp_arena);
    }


    platform->end_frame(ren);

    end_temporary_memory(&main_temp_arena);
}




