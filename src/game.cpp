#include <math.h>
#include <stdio.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>


#include <stb_image_write.h>

#include "common.h"
#include "memory.h"
#include "string.cpp"
#include "log.h"
#include "math.h"
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
integrate(Entity* entity, f32 dt)
{
    if (entity->rigidbody.inverse_mass <= 0.0f)
        return;

    entity->position += entity->rigidbody.velocity * dt;

    entity->rigidbody.velocity += entity->rigidbody.acceleration * dt;

    // Drag
    entity->rigidbody.velocity *= powf(0.5f, dt);

    entity->rigidbody.acceleration = V2(0.0f);
}

internal void
integrate(Particle* particle, f32 dt)
{
    if (particle->rigidbody.inverse_mass <= 0.0f)
        return;

    particle->position += particle->rigidbody.velocity * dt;
}

internal ParticleEmiter
create_particle_emiter(MemoryArena* arena,
                       vec2 min_vel,
                       vec2 max_vel,
                       Color color,
                       vec2 size,
                       u32 num_particles)
{
    ParticleEmiter result = {};

    result.min_vel = min_vel;
    result.max_vel = max_vel;

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


        ren->light.ambient = V3(1.0f);
        ren->light.diffuse = V3(1.0f);
        ren->light.specular = V3(1.0f);

        ren->light_pos = V3(200, 100, 0.0f);

        ren->assets = &game_state->assets;

        // NOTE: needs to be first image loaded
        ASSERT(game_state->assets.num_sprites == 0);
        ren->white_sprite = load_sprite(platform, &game_state->assets, "../assets/white.png");


        memory->debug = PushMemory(&game_state->arena, DebugState);
        sub_arena(&game_state->arena, &memory->debug->arena, Megabytes(12));


#ifdef PLATFORM_WIN32
        memory->debug->font = debug_load_font(&memory->debug->arena,
                                              platform,
                                              &trans_state->assets,
                                              "../consola.ttf",
                                              16);
#elif PLATFORM_LINUX
        memory->debug->font = debug_load_font(&memory->debug->arena,
                                              platform,
                                              &game_state->assets,
                                              "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
                                              16);
#endif


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



        game_state->particle_emiter = create_particle_emiter(&game_state->arena,
                                                             V2(-50, 50),
                                                             V2(80, 120),
                                                             COLOR(50, 100, 22, 255),
                                                             V2(10, 10),
                                                             128);

        u32 player_entity_id = add_entity(game_state);
        Entity* player_ent = get_entity(game_state, player_entity_id);

        player_ent->position = V2(100, 300);
        player_ent->render.color = COLOR(255);
        player_ent->render.size = V2(60, 60);

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

    if (button_pressed(input->enter))
    {
        global_is_edit_mode = !global_is_edit_mode;
    }


    Entity* player_ent = get_entity(game_state, game_state->player_entity_index);

    if (player_ent)
    {
        if (button_down(input->move_left))
        {
            player_ent->position.x -= 5.0f;
        }
        if (button_down(input->move_right))
        {
            player_ent->position.x += 5.0f;
        }
        if (button_down(input->move_down))
        {
            player_ent->position.y -= 5.0f;
        }
        if (button_down(input->move_up))
        {
            player_ent->position.y += 5.0f;
        }

        push_quad(render_group, 
                  &game_state->hero_sprite,
                  player_ent->position, 
                  player_ent->render.size, 
                  player_ent->render.color,
                  LAYER_MID);

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


    
    ParticleEmiter* particle_emiter = &game_state->particle_emiter;
    particle_emiter->position = player_ent->position;
    for (u32 particle_index = 0; 
         particle_index < particle_emiter->max_particles;
         ++particle_index)
    {
        Particle* particle = particle_emiter->particles + particle_index;
        integrate(particle, delta_time);

        push_quad(render_group, 
                  particle->position,
                  particle_emiter->render.size,
                  particle_emiter->render.color,
                  LAYER_MID);
    }

    for (u32 i = 0; i < 3; ++i)
    {
        if (particle_emiter->particle_index >= particle_emiter->max_particles)
        {
            particle_emiter->particle_index = 0;
        }

        Particle* particle = particle_emiter->particles + particle_emiter->particle_index;
        particle->position = particle_emiter->position;
        particle->rigidbody.velocity = random_between_two_vectors(particle_emiter->min_vel,
                                                                  particle_emiter->max_vel);
        particle->rigidbody.inverse_mass = 1;


        ++particle_emiter->particle_index;
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
        debug_ui(memory->debug, &game_state->assets, ren);

        debug_fps(memory->debug);

        debug_window_begin(memory->debug, "Debug Window");

        if (debug_submenu_begin(memory->debug, ("Main Menu")))
        {
            DebugFloatEditbox(memory->debug, &game_state->particle_emiter.min_vel, ("min particle vel"));
            DebugFloatEditbox(memory->debug, &game_state->particle_emiter.max_vel, ("max particle vel"));
        }

        if (debug_submenu_begin(memory->debug, ("Other Menu")))
        {
            debug_slider(memory->debug, -1000.0f, 1000.0f, &ren->light_pos.z, ("light z")); 
            if (debug_button(memory->debug, ("button 3")))
            {
                PRINT("button 2 clicked");
            }
            debug_cursor_sameline(memory->debug);
            if (debug_button(memory->debug, "button 4"))
            {
                PRINT("button 2 clicked");
            }

            local_persist f32 x;
            local_persist f32 y;
            debug_slider(memory->debug, -1000.0f, 1000.0f, &x, ("menu x")); 
            debug_slider(memory->debug, -1000.0f, 1000.0f, &y, ("menu y")); 
            local_persist vec3 tmp = V3(1.0f, 0.3f, 1.0f);
            local_persist vec4 tmp1 = V4(10.0f, 2.3f, 1.0f, 20.0f);
            local_persist vec2 tmp2 = V2(10.0f, 2.3f);
            DebugFloatEditbox(memory->debug, &tmp, ("test vec0"));
            DebugFloatEditbox(memory->debug, &tmp1, ("test vec1"));
            DebugFloatEditbox(memory->debug, &tmp2, ("test vec2"));

        }

        debug_window_end(memory->debug);

        debug_window_begin(memory->debug, "Another window");

        local_persist vec3 tmp = V3(1.0f, 0.3f, 1.0f);
        local_persist vec4 tmp1 = V4(10.0f, 2.3f, 1.0f, 20.0f);
        local_persist vec2 tmp2 = V2(10.0f, 2.3f);
        DebugFloatEditbox(memory->debug, &tmp, ("test vec0"));
        DebugFloatEditbox(memory->debug, &tmp1, ("test vec1"));
        DebugFloatEditbox(memory->debug, &tmp2, ("test vec2"));

        debug_window_end(memory->debug);

        process_debug_ui_interactions(memory->debug, input);

        end_temporary_memory(&memory->debug->temp_arena);
    }


    platform->end_frame(ren);

    end_temporary_memory(&main_temp_arena);
}



