// 090035212100108
#include <math.h>
#include <stdio.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include "common.h"
#include "platform.h"

Platform* g_Platform;

#include "log.h"
#include "headers/memory.h"
#include "memory.cpp"
#include "array.cpp"
#include "hashmap.cpp"
#include "string.cpp"
#include "headers/math.h"
#include "math.cpp"
#include "headers/input.h"
#include "input.cpp"
#include "headers/renderer.h"
#include "renderer.cpp"
#include "headers/debug.h"
#include "headers/game.h"
#include "render_group.cpp"
#include "asset.cpp"
#include "entity.cpp"

#include "generated_print.cpp"

#include "debug_ui.cpp"


// NOTE: Debug data

global_variable b8 global_is_edit_mode = 0;

internal void
GamePlaySound(GameSoundBuffer* game_sound, GameState* game_state)
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


internal b8
IsColliding(vec2 p1, vec2 s1, vec2 p2, vec2 s2)
{
    return (p1.x < p2.x + s2.x &&
            p1.x + s1.x > p1.x &&
            p1.y < p2.y + s2.y &&
            p1.y + s1.y > s2.y);
    
}

internal void
AddForce(Rigidbody* rigidbody, vec2 force)
{
    rigidbody->acceleration += force;
}


// F = m*a
internal void
Integrate(Rigidbody* rigidbody, Transform* transform, f32 dt)
{
    if (rigidbody->mass <= 0.0f)
        return;
    
    
    vec2 force = V2(0, -1) * 300;
    AddForce(rigidbody, force);
    
    transform->position += rigidbody->velocity * dt;
    
    rigidbody->acceleration /= rigidbody->mass;
    
    rigidbody->velocity += rigidbody->acceleration * dt;
    
    f32 drag = powf(0.5f, dt);
    rigidbody->velocity *= drag;
    
    rigidbody->acceleration = V2(0.0f);
}


internal void
Integrate(Particle* particle, f32 dt)
{
    if (particle->rigidbody.mass <= 0.0f)
        return;
    
    particle->position += particle->rigidbody.velocity * dt;
}

internal EntityId
CreateParticleEmitter(MemoryArena* arena,
                      WorldState* world,
                      vec2 min_vel,
                      vec2 max_vel,
                      u32 particle_spawn_rate,
                      Color color,
                      vec2 size,
                      u32 num_particles)
{
    // TODO:
    EntityId result; //= AddEntity(world);
    
    ParticleEmitter* pa;
    
    pa->min_vel = min_vel;
    pa->max_vel = max_vel;
    
    pa->particle_spawn_rate = particle_spawn_rate;
    
    pa->render.size = size;
    pa->render.color = color;
    
    pa->max_particles = num_particles;
    
    pa->particles = PushMemory(arena, Particle, num_particles);
    
    return result;
}

internal f32
RandomBetweenFloats(f32 min, f32 max)
{
    f32 random = ((f32)rand()) / (f32)RAND_MAX;
    f32 diff = max - min;
    f32 r = random * diff;
    return min + r;
}

internal vec2
RandomBetweenVectors(vec2 min, vec2 max)
{
    return V2(RandomBetweenFloats(min.x, max.x),
              RandomBetweenFloats(min.y, max.y));
}

internal Render
NewRender(vec2 size, Color color, f32 layer, SpriteHandle sprite)
{
    Render result = {};
    result.size = size;
    result.color = color;
    result.sprite = sprite;
    result.layer = layer;
    
    return result;
}


extern "C" PLATFORM_API void
GameMainLoop(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input)
{
    GameState* game_state = (GameState*)memory->permanent_storage;
    Renderer* ren = &game_state->renderer;
    
    if (!memory->is_initialized)
    {
        memory->is_initialized = true;
        g_Platform = &memory->platform;
        
        
        InitArena(&game_state->arena, 
                  (memory->permanent_storage_size - sizeof(GameState)),
                  (u8*)memory->permanent_storage + sizeof(GameState));
        
        InitArena(&game_state->flush_arena, 
                  (memory->temporary_storage_size),
                  (u8*)memory->temporary_storage);
        
        AssetsInit(&game_state->assets, &game_state->flush_arena);
        
        
        
        Renderer* ren = &game_state->renderer;
        
        
        ren->camera.up = V3(0, 1, 0);
        ren->camera.direction = V3(0, 0, 1);
        ren->camera.position.z = 9000.0f;
        
        ren->assets = &game_state->assets;
        
        // NOTE: needs to be first image loaded
        Assert(game_state->assets.num_sprites == 0);
        ren->white_sprite = LoadSprite(g_Platform, &game_state->assets, "../assets/white.png");
        
        
        memory->debug = PushMemory(&game_state->arena, DebugState);
        SubArena(&game_state->arena, &memory->debug->arena, Megabytes(12));
        
        UiInit(memory->debug, &memory->platform, &game_state->assets);
        
        
        game_state->minotaur_sprite = LoadSprite(g_Platform, &game_state->assets, "../assets/minotaur.png");
        game_state->hero_sprite_sheet = LoadSprite(g_Platform, &game_state->assets, 
                                                   "../assets/platform_metroidvania asset pack v1.01/herochar sprites(new)/herochar_spritesheet(new).png");
        game_state->goblin_sprite_sheet = LoadSprite(g_Platform, &game_state->assets, 
                                                     "../assets/platform_metroidvania asset pack v1.01/enemies sprites/bomber goblin/goblin_bomber_spritesheet.png");
        game_state->hero_sprite = SubspriteFromSprite(&game_state->assets,
                                                      game_state->hero_sprite_sheet,
                                                      0, 11,
                                                      16, 16);
        game_state->goblin_sprite = SubspriteFromSprite(&game_state->assets,
                                                        game_state->goblin_sprite_sheet,
                                                        0, 1,
                                                        16, 16);
        
        game_state->backgroud_sprite = LoadSprite(g_Platform, &game_state->assets,
                                                  "../assets/platform_metroidvania asset pack v1.01/tiles and background_foreground/background.png");
        
        g_Platform->InitRenderer(&game_state->renderer);
        
        
        WorldState* world = &game_state->world;
        
        InitWorld(&game_state->arena, world);
        
        EntityId player = NewEntity(world);
        AddComponent(world, player, ComponentTransform);
        AddComponent(world, player, ComponentRender);
        
        Render* player_render = (Render*)GetComponent(world, player, ComponentRender);
        *player_render = NewRender(V2(60,60), NewColor(255), LAYER_FRONT, game_state->hero_sprite);
        
        EntityId goblin = NewEntity(world);
        AddComponent(world, goblin, ComponentRender);
        AddComponent(world, goblin, ComponentTransform);
        
        Render* goblin_render = (Render*)GetComponent(world, goblin, ComponentRender);
        *goblin_render = NewRender(V2(60,60), NewColor(255), LAYER_FRONT, game_state->goblin_sprite);
        
        // EntityId player_ent = AddEntity(world);
        // EntityId goblin_ent = AddEntity(world, COMPONENT_Render | COMPONENT_Rigidbody);
        // 
        // Render* goblin_render = GetComponent(world, goblin_ent, Render);
        // 
        // Render* player_render = AddComponent(world, player_ent, Render);
        // Rigidbody* player_rigid = AddComponent(world, player_ent, Rigidbody);
        // 
        // player_rigid->mass = 1.0f;
        // 
        // game_state->particle_emitter = CreateParticleEmitter(&game_state->arena,
        // world,
        // V2(-50, 50),
        // V2(80, 120),
        // 4,
        // NewColor(50, 100, 22, 255),
        // V2(10, 10),
        // 128);
        // 
        
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
    
    TemporaryArena main_temp_arena = BeginTemporaryMemory(&game_state->flush_arena);
    
    
    RenderGroup* render_group = SetupRenderGroup(&game_state->flush_arena,
                                                 Mat4Orthographic((f32)ren->screen_width,
                                                                  (f32)ren->screen_height),
                                                 ren, 
                                                 &game_state->assets);
    
    
    {
        // Rigidbody* player_rigid = GetComponent(&game_state->world, 0, Rigidbody);
        // Render* player_render = GetComponent(&game_state->world, 0, Render);
        // Transform* player_tran = GetComponent(&game_state->world, 0, Transform);
        // 
        // if (ButtonDown(input->move_left))
        // {
        // AddForce(player_rigid, V2(-1000.0f, 0.0f));
        // }
        // if (ButtonDown(input->move_right))
        // {
        // AddForce(player_rigid, V2(1000.0f, 0.0f));
        // }
        // if (ButtonDown(input->move_up))
        // {
        // AddForce(player_rigid, V2(0.0f, 1000.0f));
        // }
        // 
        // if (player_tran->position.y <= 100.0f)
        // {
        // AddForce(player_rigid, V2(0.0f, 3000.0f));
        // }
        
    }
    
    if (ButtonPressed(input->f1))
    {
        global_is_edit_mode = !global_is_edit_mode;
    }
    
    {
        WorldState* world = &game_state->world;
        u64 key = ComponentRender | ComponentTransform;
        Array* entities = *HashMapGet(world->query_components_map,
                                      &key,
                                      Array*);
        for (u32 i = 0; i < entities->count; ++i)
        {
            EntityId entity = *ArrayGet(entities, i, EntityId);
            Transform* transform = (Transform*)GetComponent(world, entity, ComponentTransform);
            Render* render = (Render*)GetComponent(world, entity, ComponentRender);
            PushQuad(render_group,
                     transform->position,
                     render->size,
                     render->color,
                     render->layer,
                     render->sprite);
            
        }
    }
    // for (u32 i = 0; i < ENTITY_MAX; ++i)
    // {
    // if (HasComponent(&game_state->world, i, COMPONENT_Render))
    // {
    // Transform* transform = GetComponent(&game_state->world, i, Transform);
    // Render* render = GetComponent(&game_state->world, i, Render);
    // 
    // PushQuad(render_group,
    // transform->position,
    // render->size,
    // render->color,
    // render->layer,
    // render->sprite);
    // }
    // if (HasComponent(&game_state->world, i, COMPONENT_Rigidbody))
    // {
    // Transform* transform = GetComponent(&game_state->world, i, Transform);
    // Rigidbody* rigid = GetComponent(&game_state->world, i, Rigidbody);
    // 
    // Integrate(rigid, transform, delta_time);
    // }
    // }
    
    {
        
        Camera* cam = &ren->camera;
        
        if (input->mouse.wheel_delta)
        {
            f32 scroll_amount = (f32)input->mouse.wheel_delta;
            cam->position.z -= scroll_amount;
            Print("cam Z %f", cam->position.z);
            Print("scroll  %f", scroll_amount);
        }
        
        if (ButtonDown(input->move_left))
        {
            cam->position.x -= 5.0f;
        }
        if (ButtonDown(input->move_right))
        {
            cam->position.x += 5.0f;
        }
        if (ButtonDown(input->move_up))
        {
            cam->position.y += 5.0f;
        }
        if (ButtonDown(input->move_down))
        {
            cam->position.y -= 5.0f;
        }
    }
    
    
    // ParticleEmitter* particle_emitter = GetComponent(&game_state->world, 
    // game_state->particle_emitter,
    // ParticleEmitter);
    // particle_emitter->position = V2(500, 500);
    // for (u32 particle_index = 0; 
    // particle_index < particle_emitter->max_particles;
    // ++particle_index)
    // {
    // Particle* particle = particle_emitter->particles + particle_index;
    // Integrate(particle, delta_time);
    // 
    // PushQuad(render_group, 
    // particle->position,
    // particle_emitter->render.size,
    // particle_emitter->render.color,
    // LAYER_MID);
    // }
    // 
    // for (u32 i = 0; i < particle_emitter->particle_spawn_rate; ++i)
    // {
    // if (particle_emitter->particle_index >= particle_emitter->max_particles)
    // {
    // particle_emitter->particle_index = 0;
    // }
    // 
    // Particle* particle = particle_emitter->particles + particle_emitter->particle_index;
    // particle->position = particle_emitter->position;
    // particle->rigidbody.velocity = RandomBetweenVectors(particle_emitter->min_vel,
    // particle_emitter->max_vel);
    // particle->rigidbody.mass = 1;
    // 
    // 
    // ++particle_emitter->particle_index;
    // }
    
    input->mouse.position.y = ren->screen_height - input->mouse.position.y;
    ren->screen_width = memory->screen_width;
    ren->screen_height = memory->screen_height;
    
    
    
    for (u32 x = 0; x < 19; ++x)
    {
        for (u32 y = 0; y < 10; ++y)
        {
            if (game_state->tile_map[y][x])
            {
                PushQuad(render_group, 
                         V2((f32)x * game_state->tile_size, (f32)y * game_state->tile_size),
                         V2(game_state->tile_size - 4.0f, game_state->tile_size - 4.0f),
                         NewColor(75, 75, 0, 255),
                         LAYER_BACKMID);
            }
        }
    }
    
    
    if (global_is_edit_mode)
    {
        // UiStart(memory->debug, input, &game_state->assets, ren);
        // 
        // UiFps(memory->debug);
        // 
        // UiWindowBegin(memory->debug, "Window");
        // 
        // if (UiSubmenu(memory->debug, ("Particle Emitter")))
        // {
        // ParticleEmitter* emitter = GetComponent(&game_state->world,
        // game_state->particle_emitter,
        // ParticleEmitter);
        // 
        // UiFloat32Editbox(memory->debug, &emitter->min_vel, "min velocity");
        // UiFloat32Editbox(memory->debug, &emitter->max_vel, "max velocity");
        // UiInt32Editbox(memory->debug, &emitter->particle_spawn_rate, "spawn rate");
        // UiFloat32Editbox(memory->debug, &emitter->render.size, "size");
        // UiColorpicker(memory->debug, &emitter->render.color, "color mhehe");
        // local_persist f32 test = 0.5f;
        // UiSilder(memory->debug,
        // 0.1f, 
        // 10.9f,
        // &test,
        // "Test slider");
        // }
        // 
        // if (UiSubmenu(memory->debug, "Player"))
        // {
        // Rigidbody* rigid = GetComponent(&game_state->world, 0, Rigidbody);
        // Transform* trans = GetComponent(&game_state->world, 0, Transform);
        // Render* render = GetComponent(&game_state->world, 0, Render);
        // 
        // UiFloat32Editbox(memory->debug, &rigid->velocity, "velocity");
        // UiFloat32Editbox(memory->debug, &rigid->mass, "mass");
        // 
        // UiFloat32Editbox(memory->debug, &trans->position, "position");
        // UiFloat32Editbox(memory->debug, &trans->scale, "scale");
        // UiFloat32Editbox(memory->debug, &trans->rotation, "rotation");
        // 
        // UiFloat32Editbox(memory->debug, &render->size, "size");
        // UiColorpicker(memory->debug, &render->color, "color");
        // }
        // 
        // UiWindowEnd(memory->debug);
        // 
        // EndTemporaryMemory(&memory->debug->temp_arena);
    }
    
    
    
    g_Platform->EndFrame(ren);
    
    EndTemporaryMemory(&main_temp_arena);
}




