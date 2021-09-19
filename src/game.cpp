#include <math.h>
#include <stdio.h>

#define DLL_FILE

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

global_variable b32 global_is_edit_mode = 0;

#include "generated/memory.h"
#include "memory.cpp"
#include "string.cpp"
#include "generated/math.h"
#include "math.cpp"
#include "generated/input.h"
#include "input.cpp"
#include "file_parse.cpp"
#include "generated/renderer.h"
#include "asset.cpp"
#include "renderer.cpp"
#include "generated/dev_ui.h"
#include "dev_ui.cpp"
#include "generated/debug.h"
#include "debug.cpp"
#include "generated/game.h"
#include "entity.cpp"
#include "generated_print.c"
#include "render_group.cpp"




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


internal b32
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
GameInit(GameMemory* memory, Renderer2D* renderer)
{
    GameState* game_state = (GameState*)memory->permanent_storage;
    game_state->renderer = renderer;
    
    DebugState* debug = (DebugState*)memory->debug_storage;
    debug->GetPrefCounter = memory->platform.GetPrefCounter;
    debug->GetElapsedSeconds = memory->platform.GetElapsedSeconds;
    global_debug_state = debug;
    
    
    InitArena(&debug->arena, 
              (memory->debug_storage_size - sizeof(DebugState)),
              (u8*)memory->debug_storage + sizeof(DebugState));
    
    
    InitArena(&game_state->arena, 
              (memory->permanent_storage_size - sizeof(GameState)),
              (u8*)memory->permanent_storage + sizeof(GameState));
    
    InitArena(&game_state->flush_arena, 
              (memory->temporary_storage_size),
              (u8*)memory->temporary_storage);
    
    AssetsInit(&memory->platform, &game_state->assets, &game_state->flush_arena);
    
    game_state->font = LoadFontTest(&memory->platform, &game_state->assets, "../assets/fonts/consola.ttf", 32);
    
    
    renderer->assets = &game_state->assets;
    
    memory->platform.RendererInit(game_state->renderer);
    game_state->render_group = CreateRenderGroup(&game_state->flush_arena, Mat4Orthographic((f32)renderer->screen_width, (f32)renderer->screen_height),
                                                 CreateCamera(Vec3Up(), Vec3Forward(), V3(0.0f, 0.0f, 200.0f)),
                                                 game_state->renderer,
                                                 &game_state->assets,
                                                 Megabytes(12));
    
    // TODO: temporary
    renderer->camera = &game_state->render_group->setup.camera;
    
    DevUiInit(&debug->dev_ui, &debug->arena, renderer, &memory->platform, &game_state->assets);
    
    
    WorldState* world = &game_state->world;
#if 0
    ParticleEmitter* player_particles = GetComponent(world, player, ParticleEmitter);
    *player_particles = CreateParticleEmitter(&game_state->flush_arena, world, V2(-100), V2(100), 4,
                                              NewColor(255), V2(30), 10000);
#endif
    
}


extern "C" PLATFORM_API void
GameUpdate(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input)
{
#ifdef GAME_DEBUG
    global_debug_state = (DebugState*)memory->debug_storage;
#endif
    
    GameState* game_state = (GameState*)memory->permanent_storage;
    DebugState* debug = (DebugState*)memory->debug_storage;
    
    DEBUG_FRAME_START(debug, input, &game_state->assets, game_state->renderer);
    PROFILE_FUNCTION();
    
    
    game_state->delta_time = delta_time;
    Renderer2D* ren = game_state->renderer;
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
    PushText(game_state->render_group, &game_state->font, "testing hello ahaha", V2(600, 200), LAYER_FRONT);
    
    PushMesh(game_state->render_group, V3(-100, -100, 0), V3(50), NewColor(255), MESH_CUBE);
    PushMesh(game_state->render_group, V3(100, 100, 0), V3(50), NewColor(255), MESH_SPHERE);
    
    PushMesh(game_state->render_group, ren->light.position, V3(10), ColorYellow, MESH_CUBE);
    
    PushClearScreen(game_state->render_group, NewColor(120, 75, 25, 255));
    
    memory->platform.RendererDraw(ren);
    
    if (global_is_edit_mode)
    {
        
        UiFps(&debug->dev_ui, debug->game_fps);
        
        UiWindowBegin(&debug->dev_ui, "Window 1");
        
        if (UiSubmenu(&debug->dev_ui, "Camera"))
        {
            Camera* camera = &game_state->render_group->setup.camera;
            UiFloat32Editbox((&debug->dev_ui), &camera->up, "up");
            UiFloat32Editbox((&debug->dev_ui), &camera->direction, "direction");
            UiFloat32Editbox((&debug->dev_ui), &camera->position, "position");
        }
        
        if (UiSubmenu(&debug->dev_ui, "Player"))
        {
            /*
                        EntityId player = 0;
                        Rigidbody* rigid = GetComponent(&game_state->world, player, Rigidbody);
                        Transform* trans = GetComponent(&game_state->world, player, Transform);
                        Render* render = GetComponent(&game_state->world, player, Render);
                        
                        UiCheckbox(&debug->dev_ui, &game_state->is_free_camera, "free camera");
                        UiFloat32Editbox(&debug->dev_ui, &rigid->velocity, "velocity");
                        UiFloat32Editbox(&debug->dev_ui, &rigid->mass, "mass");
                        
                        UiFloat32Editbox(&debug->dev_ui, &trans->position, "position");
                        UiFloat32Editbox(&debug->dev_ui, &trans->rotation, "rotation");
                        UiFloat32Editbox(&debug->dev_ui, &trans->scale, "scale");
                        
                        UiColorpicker(&debug->dev_ui, &render->color, "color");
                        
            */
            
        }
        
        if (UiButton(&debug->dev_ui, "Save Ui Config", V2(200, 200), V2(150, 20)))
        {
            WriteUiConfig(&memory->platform, &debug->dev_ui);
        }
        
        
        UiWindowEnd(&debug->dev_ui);
        
        UiWindowBegin(&debug->dev_ui, "Window 2");
        
        if (UiSubmenu(&debug->dev_ui, "Camera settings"))
        {
            Camera* cam = &game_state->render_group->setup.camera;
            UiFloat32Editbox((&debug->dev_ui), &cam->position, "position");
            UiCheckbox(&debug->dev_ui, &game_state->is_free_camera, "free camera");
        }
        
        if (UiSubmenu(&debug->dev_ui, "Particle Settings"))
        {
            /*
                        ParticleEmitter* pe = GetComponent(&game_state->world, 0, ParticleEmitter);
                        UiFloat32Editbox(&debug->dev_ui, &pe->position, "position");
                        UiFloat32Editbox(&debug->dev_ui, &pe->min_vel, "min velocity");
                        UiFloat32Editbox(&debug->dev_ui, &pe->max_vel, "max velocity");
                        UiFloat32Editbox(&debug->dev_ui, &pe->size, "size");
                        UiInt32Editbox(&debug->dev_ui, &pe->particle_spawn_rate, "spawn rate");
                        UiColorpicker(&debug->dev_ui, &pe->color, "color");
            */
        }
        if (UiSubmenu(&debug->dev_ui, "3D rendering"))
        {
            if (UiSubmenu(&debug->dev_ui, "Lighting"))
            {
                UiFloat32Editbox((&debug->dev_ui), &ren->light.position, "Light position");
                UiFloat32Editbox((&debug->dev_ui), &ren->light.ambient, "Ambient light");
                UiFloat32Editbox((&debug->dev_ui), &ren->light.diffuse, "Diffuse light");
                UiFloat32Editbox((&debug->dev_ui), &ren->light.specular, "Specular light");
            }
            if (UiSubmenu(&debug->dev_ui, "Material"))
            {
                Mesh* mesh = GetMesh(&game_state->assets, MESH_CUBE);
                
                UiFloat32Editbox((&debug->dev_ui), &mesh->material.ambient, "ambient");
                UiFloat32Editbox((&debug->dev_ui), &mesh->material.diffuse, "diffuse");
                UiFloat32Editbox((&debug->dev_ui), &mesh->material.specular, "specular");
                UiFloat32Editbox((&debug->dev_ui), &mesh->material.shininess, "shininess");
            }
        }
        
        UiWindowEnd(&debug->dev_ui);
        
        if (!debug->have_read_config)
        {
            ReadUiConfig(&memory->platform, &debug->dev_ui);
            debug->have_read_config = true;
        }
        
    }
    
    memory->platform.RendererDraw(ren);
}