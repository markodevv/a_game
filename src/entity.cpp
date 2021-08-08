enum ComponentType
{
    ComponentTransform       = 1,
    ComponentRigidbody       = 2,
    ComponentRender          = 4,
    ComponentParticleEmitter = 8,
};

internal EntityId
NewEntity(WorldState* world)
{
    return world->entity_count++;
}



#define InitComponent(world, comp, type) \
_InitComponent(world, comp, sizeof(type), #type);

internal void
_InitComponent(WorldState* world, ComponentType type, u32 size, char* name)
{
    u64 id = Log2(type);
    world->component_infos[id].size = size;
    world->component_infos[id].name = name;
}

#define RegisterSystem(world, components, num_components, func) \
_RegisterSystem(world, components, num_components, func, #func);

internal void
_RegisterSystem(WorldState* world, 
                ComponentType* components, 
                u8 num_components, 
                SystemFunc system_function,
                char* system_name)
{
    System* system = &world->systems[world->num_systems];
    for (u32 i = 0; i < num_components; ++i)
    {
        system->signature |= components[i];
    }
    system->entities = CreateArray(10, EntityId);
    system->Update = system_function;
    system->id = world->num_systems;
    system->entity_id_to_array_id = CreateHashMap(512, HashUInt64, u32);
    system->name = system_name;
    
    world->num_systems++;
}

internal void 
InitWorld(MemoryArena* arena, WorldState* world)
{
    InitComponent(world, ComponentTransform, Transform);
    InitComponent(world, ComponentRender, Render);
    InitComponent(world, ComponentRigidbody, Rigidbody);
    InitComponent(world, ComponentParticleEmitter, ParticleEmitter);
    
    for (u32 i = 0; i < NUM_COMPONENTS; ++i)
    {
        ComponentInfo info = world->component_infos[i];
        world->components[i].data = _PushMemory(arena, info.size, ENTITY_MAX);
    }
    
}

#define AddComponent(world, entity, type) \
_AddComponent(world, entity, Component##type);

internal void
_AddComponent(WorldState* world, EntityId entity, ComponentType type)
{
    if ((world->entity_masks[entity] & type) == type)
    {
        u64 index = Log2(type);
        ComponentInfo info = world->component_infos[index];
        Print("Entity %u already has component %s!", entity, info.name);
        return;
    }
    world->entity_masks[entity] |= type;
    
    for (u32 i = 0; i < world->num_systems; ++i)
    {
        u64 entity_mask = world->entity_masks[entity];
        System* system = &world->systems[i];
        if ((system->signature & entity_mask) == system->signature)
        {
            u32* id = HashMapGet(system->entity_id_to_array_id, &entity, u32);
            if (!id)
            {
                EntityId* et = ArrayAdd(system->entities, EntityId);
                *et = entity;
                u32* id = HashMapPut(system->entity_id_to_array_id, &entity, u32);
                *id = system->entities->count - 1;
            }
        }
    }
}

internal void
RemoveComponent(WorldState* world, EntityId entity, ComponentType type)
{
    if ((world->entity_masks[entity] & type) != type)
    {
        u64 index = Log2(type);
        ComponentInfo info = world->component_infos[index];
        Print("Entity %u has no component %s!", entity, info.name);
        return;
    }
    for (u32 i = 0; i < world->num_systems; ++i)
    {
        System* system = &world->systems[i];
        if ((system->signature & type) == type)
        {
            u32 id = *HashMapGet(system->entity_id_to_array_id, &entity, u32);
            ArrayRemove(system->entities, id, EntityId);
            HashMapRemove(system->entity_id_to_array_id, &entity, u32);
        }
    }
    world->entity_masks[entity] &= ~type;
}

#define GetComponent(world, entity, type) \
(type*)_GetComponent(world, entity, Component##type);


internal void*
_GetComponent(WorldState* world, EntityId entity, ComponentType type)
{
    u64 index = Log2(type);
    ComponentInfo info = world->component_infos[index];
    return ((u8*)world->components[index].data) + (entity * info.size);
}

internal void
UpdateSystems(GameState* game_state)
{
    for (u32 i = 0; i < game_state->world.num_systems; ++i)
    {
        game_state->world.systems[i].Update(game_state, game_state->world.systems[i].entities);
    }
}
