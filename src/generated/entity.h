#pragma once

// @Print
typedef struct Render Render;
struct Render
{
    // @NoPrint
    SpriteID sprite;
    Color color;
    f32 layer;
};
// @Print
typedef struct Transform Transform;
struct Transform
{
    vec2 position;
    vec2 scale;
    vec2 rotation;
};
// @Print
typedef struct Rigidbody Rigidbody;
struct Rigidbody
{
    vec2 velocity;
    vec2 acceleration;

    f32 mass;
};
// @Print
typedef struct Particle Particle;
struct Particle
{
    vec2 position;
    vec2 velocity;
    vec2 acceleration;
    vec2 size;
};
typedef struct ParticleEmitter ParticleEmitter;
struct ParticleEmitter
{
    vec2 min_vel;
    vec2 max_vel;

    f32 drag;

    Color color;
    vec2 size;

    u32 particle_spawn_rate;

    vec2 position;

    Particle *particles;
    u32 particle_index;
    u32 max_particles;
};
#define ENTITY_MAX (10000)
#define NUM_COMPONENTS (4)
typedef struct ComponentInfo ComponentInfo;
struct ComponentInfo
{
    u32 size;
    char *name;
};
typedef struct Component Component;
struct Component
{
    void *data;
};
typedef struct System System;
struct System
{
    u32 id;
    Array *entities;
    u64 signature;
    SystemFunc Update;
    HashMap entity_id_to_array_id;
    char *name;
};
