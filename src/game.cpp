#include <math.h>
#include <stdio.h>

#define GAME_FILE

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#ifdef GAME_DEBUG
struct DebugState* global_debug_state;
#endif

#include "log.h"
#include "platform.h"
#include "generated/memory.h"
#include "memory.cpp"
#include "array.cpp"
#include "hashmap.cpp"
#include "string.cpp"
#include "generated/math.h"
#include "math.cpp"
#include "generated/input.h"
#include "input.cpp"
#include "generated/renderer.h"
#include "renderer.cpp"
#include "generated/debug.h"
#include "debug_profiler.cpp"
#include "generated/game.h"
#include "render_group.cpp"
#include "generated_print.c"
#include "asset.cpp"
#include "entity.cpp"
#include "debug_ui.cpp"



Platform g_Platform;

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


internal ParticleEmitter
CreateParticleEmitter(MemoryArena* arena,
                      WorldState* world,
                      vec2 min_vel,
                      vec2 max_vel,
                      u32 particle_spawn_rate,
                      Color color,
                      vec2 size,
                      u32 num_particles)
{
    ParticleEmitter pa = {};
    
    pa.min_vel = min_vel;
    pa.max_vel = max_vel;
    
    pa.particle_spawn_rate = particle_spawn_rate;
    
    pa.color = color;
    pa.size = size;
    
    pa.max_particles = num_particles;
    
    pa.particles = PushMemory(arena, Particle, num_particles);
    
    return pa;
}

internal vec2
RandomBetweenVectors(vec2 min, vec2 max)
{
    return V2(RandomRange(min.x, max.x),
              RandomRange(min.y, max.y));
}

internal void
ParticleUpdate(GameState* game_state, Array* entities)
{
    WorldState* world = &game_state->world;
    float dt = game_state->delta_time;
    
    for (u32 i = 0; i < entities->count; ++i)
    {
        EntityId entity = *ArrayGet(entities, i, EntityId);
        ParticleEmitter* emitter = GetComponent(world, entity, ParticleEmitter);
        Transform* transform = GetComponent(world, entity, Transform);
        emitter->position = transform->position;
        
        for (u32 particle_index = 0; 
             particle_index < emitter->max_particles;
             ++particle_index)
        {
            Particle* particle = emitter->particles + particle_index;
            // Simulate
            particle->position += particle->velocity * dt;
            
            //Render
            PushQuad(game_state->render_group, 
                     particle->position,
                     emitter->size,
                     emitter->color,
                     LAYER_MID);
        }
        
        // Spawn new particles
        for (u32 i = 0; i < emitter->particle_spawn_rate; ++i)
        {
            if (emitter->particle_index >= emitter->max_particles)
            {
                emitter->particle_index = 0;
            }
            
            Particle* particle = emitter->particles + emitter->particle_index;
            particle->position = emitter->position;
            particle->velocity = RandomBetweenVectors(emitter->min_vel,
                                                      emitter->max_vel);
            
            ++emitter->particle_index;
        }
    }
}

internal void 
RenderUpdate(GameState* game_state, Array* entities)
{
#if 0
    WorldState* world = &game_state->world;
    for (u32 i = 0; i < entities->count; ++i)
    {
        EntityId entity = *ArrayGet(entities, i, EntityId);
        Transform* transform = GetComponent(world, entity, Transform);
        Render* render = GetComponent(world, entity, Render);
        
        PushQuad(game_state->render_group,
                 transform->position,
                 transform->scale,
                 render->color,
                 render->layer,
                 render->sprite);
        
    }
#endif
}

internal void 
PhysicsUpdate(GameState* game_state, Array* entities)
{
    PROFILE_FUNCTION();
    
    WorldState* world = &game_state->world;
    f32 dt = game_state->delta_time;
    
    for (u32 i = 0; i < entities->count; ++i)
    {
        EntityId entity = *ArrayGet(entities, i, EntityId);
        Transform* transform = GetComponent(world, entity, Transform);
        Rigidbody* rigidbody = GetComponent(world, entity, Rigidbody);
        
        if (rigidbody->mass <= 0.0f)
            return;
        
        transform->position += rigidbody->velocity * dt;
        
        rigidbody->acceleration /= rigidbody->mass;
        
        rigidbody->velocity += rigidbody->acceleration * dt;
        
        f32 drag = powf(0.5f, dt);
        rigidbody->velocity *= drag;
        
        rigidbody->acceleration = V2(0.0f);
        
    }
}

internal void
InitGrid(WorldState* world)
{
    world->grid_width = 50;
    world->grid_height = 50;
    float offset = 4;
    
    for (u16 x = 0; x < world->grid_width; ++x)
    {
        for (u16 y = 0; y < world->grid_height; ++y)
        {
            EntityId chunk = NewEntity(world);
            
            AddComponent(world, chunk, Transform);
            AddComponent(world, chunk, Render);
            
            Render* render = GetComponent(world, chunk, Render);
            Transform* transform = GetComponent(world, chunk, Transform);
            
            vec2 size = V2(40, 40);
            
            *render = 
            {
                0,
                NewColor(120, 20, 40, 255),
                LAYER_BACKMID,
            };
            *transform = 
            {
                V2((size.x + offset) * x, (size.y + offset) * y),
                size,
                V2(0),
            };
        }
    }
}

internal void 
InitGame(GameMemory* memory, GameState* game_state, GameInput* input)
{
    Renderer2D* ren = &game_state->renderer;
    
    InitArena(&game_state->arena, 
              (memory->permanent_storage_size - sizeof(GameState)),
              (u8*)memory->permanent_storage + sizeof(GameState));
    
    InitArena(&game_state->flush_arena, 
              (memory->temporary_storage_size),
              (u8*)memory->temporary_storage);
    
    AssetsInit(&game_state->assets, &game_state->flush_arena);
    
    
    //ren->camera.up = V3(0, 1, 0);
    //ren->camera.direction = V3(0, 0, 1);
    //ren->camera.position.z = 9000.0f;
    
    ren->assets = &game_state->assets;
    
    // NOTE: needs to be first image loaded
    Assert(game_state->assets.num_sprites == 0);
    
    
    LoadSpriteWorkData sprites[4];
    
    sprites[0].assets = &game_state->assets;
    sprites[0].sprite_path = "../assets/white.png";
    sprites[1].assets = &game_state->assets;
    sprites[1].sprite_path  = "../assets/red.png";
    sprites[2].assets = &game_state->assets;
    sprites[2].sprite_path = "../assets/green.png";
    sprites[3].assets = &game_state->assets;
    sprites[3].sprite_path = "../assets/blue.png";
    
    g_Platform.PushWorkEntry(g_Platform.work_queue, LoadSpriteWork, &sprites[0]);
    g_Platform.PushWorkEntry(g_Platform.work_queue, LoadSpriteWork, &sprites[1]);
    g_Platform.PushWorkEntry(g_Platform.work_queue, LoadSpriteWork, &sprites[2]);
    g_Platform.PushWorkEntry(g_Platform.work_queue, LoadSpriteWork, &sprites[3]);
    
    g_Platform.WaitForWorkers(g_Platform.work_queue);
    
    LoadOBJModel(&g_Platform, &game_state->assets, "../assets/models/cube.obj");
    
    g_Platform.InitRenderer(&game_state->renderer);
    game_state->render_group = CreateRenderGroup(&game_state->flush_arena, Mat4Orthographic((f32)ren->screen_width, (f32)ren->screen_height),
                                                 CreateCamera(Vec3Up(), Vec3Forward(), V3(0.0f, 0.0f, 200.0f)),
                                                 &game_state->renderer,
                                                 &game_state->assets);
    
    // TODO: temporary
    ren->camera = &game_state->render_group->setup.camera;
    
    UiInit(memory->debug, ren, &memory->platform, &game_state->assets);
    
    
    WorldState* world = &game_state->world;
    
    InitWorld(&game_state->arena, world);
    ComponentType components[] = 
    {
        ComponentTransform,
        ComponentRender,
    };
    RegisterSystem(world, components, ArrayCount(components), RenderUpdate);
    
    components[0] = ComponentTransform;
    components[1] = ComponentRigidbody;
    
    RegisterSystem(world, components, ArrayCount(components), PhysicsUpdate);
    
    ComponentType comps[] = {ComponentParticleEmitter};
    RegisterSystem(world, comps, ArrayCount(comps), ParticleUpdate);
    
    EntityId player = NewEntity(world);
    AddComponent(world, player, Transform);
    AddComponent(world, player, Render);
    AddComponent(world, player, Rigidbody);
    AddComponent(world, player, ParticleEmitter);
    
    /*
        ParticleEmitter* player_particles = GetComponent(world, player, ParticleEmitter);
        *player_particles = CreateParticleEmitter(&game_state->flush_arena, world, V2(-100), V2(100), 4,
     NewColor(255), V2(30), 10000);
    */
    
    Transform* player_transform = GetComponent(world, player, Transform);
    *player_transform = {
        V2(200, 200),
        V2(60, 60),
        V2(0),
    };
    Render* player_render = GetComponent(world, player, Render);
    *player_render = {
        0,
        NewColor(255),
        LAYER_FRONT
    };
    
    Rigidbody* player_rigidbody = GetComponent(world, player, Rigidbody);
    
    *player_rigidbody = {
        V2(0),
        V2(0),
        1.0f,
    };
    
    EntityId goblin = NewEntity(world);
    AddComponent(world, goblin, Render);
    AddComponent(world, goblin, Transform);
    
    Render* goblin_render = GetComponent(world, goblin, Render);
    *goblin_render = {0, NewColor(255), LAYER_FRONT};
    
    
    InitGrid(world);
    
}


extern "C" PLATFORM_API void
GameMainLoop(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input)
{
    GameState* game_state = (GameState*)memory->permanent_storage;
    game_state->delta_time = delta_time;
    Renderer2D* ren = &game_state->renderer;
    input->mouse.position.y = ren->screen_height - input->mouse.position.y;
    
    if (ren->screen_width != memory->screen_width ||
        ren->screen_height != memory->screen_height)
    {
        ren->screen_width = memory->screen_width;
        ren->screen_height = memory->screen_height;
        
        for (u32 i = 0; i < ren->render_group_count; ++i)
        {
            ren->render_groups[i].setup.projection = Mat4Orthographic(ren->screen_width,
                                                                      ren->screen_height);
        }
    }
    
    
    if (!memory->is_initialized)
    {
        global_debug_state = memory->debug;
        g_Platform = memory->platform;
        InitGame(memory, game_state, input);
        memory->is_initialized = true;
    }
    PROFILE_FUNCTION();
    
    
    
    
    UpdateSystems(game_state);
    
    if (!game_state->is_free_camera)
    {
        EntityId player = 0;
        Rigidbody* player_rigid = GetComponent(&game_state->world, player, Rigidbody);
        Transform* player_tran = GetComponent(&game_state->world, player, Transform);
        
        if (ButtonDown(input, BUTTON_LEFT))
        {
            AddForce(player_rigid, V2(-1000.0f, 0.0f));
        }
        if (ButtonDown(input, BUTTON_RIGHT))
        {
            AddForce(player_rigid, V2(1000.0f, 0.0f));
        }
        if (ButtonDown(input, BUTTON_UP))
        {
            AddForce(player_rigid, V2(0.0f, 1000.0f));
        }
        if (ButtonDown(input, BUTTON_DOWN))
        {
            AddForce(player_rigid, V2(0.0f, -1000.0f));
        }
        
    }
    
    // TOggle edit mode
    if (ButtonPressed(input, BUTTON_F1))
    {
        global_is_edit_mode = !global_is_edit_mode;
        if (global_is_edit_mode)
        {
            game_state->render_group->setup.projection = Mat4Perspective(ren->screen_width,
                                                                         ren->screen_height,
                                                                         90.0f,
                                                                         1.0f,
                                                                         100000.0f);
        }
        else
        {
            game_state->render_group->setup.projection = Mat4Orthographic((f32)ren->screen_width, (f32)ren->screen_height);
        }
    }
    
    {
        
        Camera* cam = &game_state->render_group->setup.camera;
        
        if (input->mouse.wheel_delta)
        {
            f32 scroll_amount = (f32)input->mouse.wheel_delta;
            cam->position.z -= scroll_amount;
        }
        
        if (ButtonDown(input, BUTTON_LEFT))
        {
            cam->position.x -= 5.0f;
        }
        if (ButtonDown(input, BUTTON_RIGHT))
        {
            cam->position.x += 5.0f;
        }
        if (ButtonDown(input, BUTTON_UP))
        {
            cam->position.y += 5.0f;
        }
        if (ButtonDown(input, BUTTON_DOWN))
        {
            cam->position.y -= 5.0f;
        }
    }
    
    
    
    
    if (global_is_edit_mode)
    {
        DebugState* debug = memory->debug;
        UiStart(debug, input, &game_state->assets, ren);
        
        UiFps(debug);
        
        UiWindowBegin(debug, "Window 1");
        
        if (UiSubmenu(debug, "Camera"))
        {
            Camera* camera = &game_state->render_group->setup.camera;
            UiFloat32Editbox(debug, &camera->up, "up");
            UiFloat32Editbox(debug, &camera->direction, "direction");
            UiFloat32Editbox(debug, &camera->position, "position");
        }
        
        if (UiSubmenu(debug, "Player"))
        {
            EntityId player = 0;
            Rigidbody* rigid = GetComponent(&game_state->world, player, Rigidbody);
            Transform* trans = GetComponent(&game_state->world, player, Transform);
            Render* render = GetComponent(&game_state->world, player, Render);
            
            UiCheckbox(debug, &game_state->is_free_camera, "free camera");
            UiFloat32Editbox(debug, &rigid->velocity, "velocity");
            UiFloat32Editbox(debug, &rigid->mass, "mass");
            
            UiFloat32Editbox(debug, &trans->position, "position");
            UiFloat32Editbox(debug, &trans->rotation, "rotation");
            UiFloat32Editbox(debug, &trans->scale, "scale");
            
            //Log(&game_state->render_group->setup.camera);
            UiColorpicker(debug, &render->color, "color");
            
            if (UiButton(debug, "Save Ui Config", V2(200, 200), V2(150, 20)))
            {
                WriteUiConfig(debug);
            }
            
        }
        
        
        UiWindowEnd(memory->debug);
        
        UiWindowBegin(debug, "Window 2");
        
        if (UiSubmenu(debug, "Camera settings"))
        {
            Camera* cam = &game_state->render_group->setup.camera;
            UiFloat32Editbox(debug, &cam->position, "position");
            UiCheckbox(debug, &game_state->is_free_camera, "free camera");
        }
        
        if (UiSubmenu(debug, "Particle Settings"))
        {
            ParticleEmitter* pe = GetComponent(&game_state->world, 0, ParticleEmitter);
            UiFloat32Editbox(debug, &pe->position, "position");
            UiFloat32Editbox(debug, &pe->min_vel, "min velocity");
            UiFloat32Editbox(debug, &pe->max_vel, "max velocity");
            UiFloat32Editbox(debug, &pe->size, "size");
            UiInt32Editbox(debug, &pe->particle_spawn_rate, "spawn rate");
            UiColorpicker(debug, &pe->color, "color");
        }
        
        UiWindowEnd(memory->debug);
        
        UiWindowBegin(debug, "Window 3");
        
        UiWindowEnd(memory->debug);
        
        UiEnd(debug);
    }
    
    
    g_Platform.RendererEndFrame(ren);
    
}




