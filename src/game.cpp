#include <math.h>
#include <stdio.h>

#define GAME_FILE

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

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
#include "debug.cpp"
#include "generated/game.h"
#include "entity.cpp"
#include "generated_print.c"
#include "file_parse.cpp"
#include "asset.cpp"
#include "render_group.cpp"
#include "dev_ui.cpp"



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

extern "C" PLATFORM_API void
GameInit(GameMemory* memory)
{
    GameState* game_state = (GameState*)memory->permanent_storage;
    Renderer2D* ren = &game_state->renderer;
    
    global_debug_state = memory->debug;
    g_Platform = memory->platform;
    
    InitArena(&game_state->arena, 
              (memory->permanent_storage_size - sizeof(GameState)),
              (u8*)memory->permanent_storage + sizeof(GameState));
    
    InitArena(&game_state->flush_arena, 
              (memory->temporary_storage_size),
              (u8*)memory->temporary_storage);
    
    AssetsInit(&game_state->assets, &game_state->flush_arena);
    
    game_state->font = LoadFontTest(&game_state->assets, "../assets/fonts/consola.ttf", 32);
    
    
    ren->assets = &game_state->assets;
    
    g_Platform.InitRenderer(&game_state->renderer);
    game_state->render_group = CreateRenderGroup(&game_state->flush_arena, Mat4Orthographic((f32)ren->screen_width, (f32)ren->screen_height),
                                                 CreateCamera(Vec3Up(), Vec3Forward(), V3(0.0f, 0.0f, 200.0f)),
                                                 &game_state->renderer,
                                                 &game_state->assets);
    
    // TODO: temporary
    ren->camera = &game_state->render_group->setup.camera;
    
    DevUiInit(memory->debug, ren, &memory->platform, &game_state->assets);
    
    
    WorldState* world = &game_state->world;
#if 0
    ParticleEmitter* player_particles = GetComponent(world, player, ParticleEmitter);
    *player_particles = CreateParticleEmitter(&game_state->flush_arena, world, V2(-100), V2(100), 4,
                                              NewColor(255), V2(30), 10000);
#endif
    
}


extern "C" PLATFORM_API void
GameMainLoop(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input)
{
    PROFILE_FUNCTION();
    
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
    
    
#if 0
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
#endif
    
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
    
    for (u32 entityID = 0; entityID < ENTITY_MAX; ++entityID)
    {
        Entity* entity = GetEntity(&game_state->world, entityID);
        
        if ((entity->flags & RENDER) == RENDER)
        {
            PushMesh(game_state->render_group, 
                     entity->position, 
                     entity->scale, 
                     entity->color, 
                     entity->meshID);
        }
    }
    
    PushText(game_state->render_group, &game_state->font, "testing hello", V2(200, 200), LAYER_FRONT);
    
    PushMesh(game_state->render_group, V3(-100, -100, 0), V3(50), NewColor(255), MESH_CUBE);
    PushMesh(game_state->render_group, V3(100, 100, 0), V3(50), NewColor(255), MESH_SPHERE);
    
    PushMesh(game_state->render_group, ren->light.position, V3(10), ColorYellow, MESH_CUBE);
    
    PushClearScreen(game_state->render_group, NewColor(120, 75, 25, 255));
    
    g_Platform.RendererDraw(ren);
    
    if (global_is_edit_mode)
    {
        DebugState* debug = memory->debug;
        DevUiStart(debug, input, &game_state->assets, ren);
        
        UiFps(debug);
        
        UiProfilerWindow(debug);
        
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
            /*
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
                        
                        UiColorpicker(debug, &render->color, "color");
                        
            */
            
        }
        
        if (UiButton(debug, "Save Ui Config", V2(200, 200), V2(150, 20)))
        {
            WriteUiConfig(debug);
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
            /*
                        ParticleEmitter* pe = GetComponent(&game_state->world, 0, ParticleEmitter);
                        UiFloat32Editbox(debug, &pe->position, "position");
                        UiFloat32Editbox(debug, &pe->min_vel, "min velocity");
                        UiFloat32Editbox(debug, &pe->max_vel, "max velocity");
                        UiFloat32Editbox(debug, &pe->size, "size");
                        UiInt32Editbox(debug, &pe->particle_spawn_rate, "spawn rate");
                        UiColorpicker(debug, &pe->color, "color");
            */
        }
        if (UiSubmenu(debug, "3D rendering"))
        {
            if (UiSubmenu(debug, "Lighting"))
            {
                UiFloat32Editbox(debug, &ren->light.position, "Light position");
                UiFloat32Editbox(debug, &ren->light.ambient, "Ambient light");
                UiFloat32Editbox(debug, &ren->light.diffuse, "Diffuse light");
                UiFloat32Editbox(debug, &ren->light.specular, "Specular light");
            }
            if (UiSubmenu(debug, "Material"))
            {
                Mesh* mesh = GetMesh(&game_state->assets, MESH_CUBE);
                
                UiFloat32Editbox(debug, &mesh->material.ambient, "ambient");
                UiFloat32Editbox(debug, &mesh->material.diffuse, "diffuse");
                UiFloat32Editbox(debug, &mesh->material.specular, "specular");
                UiFloat32Editbox(debug, &mesh->material.shininess, "shininess");
            }
            
        }
        
        UiWindowEnd(debug);
        
        
        if (!debug->have_read_config)
        {
            ReadUiConfig(debug);
            debug->have_read_config = true;
        }
        
    }
    
    g_Platform.RendererDraw(ren);
}