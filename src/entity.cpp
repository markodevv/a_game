enum ComponentType
{
    ComponentTransform = 1,
    ComponentRigidBody = 2,
    ComponentRender    = 4,
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

internal void
RegisterSystem(WorldState* world, 
               ComponentType* components, 
               u8 num_components, 
               SystemFunc system_function)
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
    world->num_systems++;
}

internal void 
InitWorld(MemoryArena* arena, WorldState* world)
{
    InitComponent(world, ComponentTransform, Transform);
    InitComponent(world, ComponentRender, Render);
    InitComponent(world, ComponentRigidBody, Rigidbody);
    
    for (u32 i = 0; i < NUM_COMPONENTS; ++i)
    {
        ComponentInfo info = world->component_infos[i];
        world->components[i].data = _PushMemory(arena, info.size, ENTITY_MAX);
    }
    
}

internal void
AddComponent(WorldState* world, EntityId entity, ComponentType type)
{
    if ((world->entity_masks[entity] & type) == type)
    {
        u64 index = Log2(type);
        ComponentInfo info = world->component_infos[index];
        Print("Entity %zu already has component %s!", entity, info.name);
        return;
    }
    world->entity_masks[entity] |= type;
    
    for (u32 i = 0; i < world->num_systems; ++i)
    {
        u64 entity_mask = world->entity_masks[entity];
        System* system = &world->systems[i];
        if ((system->signature & entity_mask) == system->signature)
        {
            EntityId* et = ArrayAdd(system->entities, EntityId);
            *et = entity;
            u32* id = HashMapPut(system->entity_id_to_array_id, &entity, u32);
            *id = system->entities->count - 1;
            break;
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
        Print("Entity %zu has no component %s!", entity, info.name);
        return;
    }
    for (u32 i = 0; i < world->num_systems; ++i)
    {
        System* system = &world->systems[i];
        u64 entity_mask = world->entity_masks[entity];
        if ((system->signature & entity_mask) == system->signature)
        {
            u32 id = *HashMapGet(system->entity_id_to_array_id, &entity, u32);
            ArrayRemove(system->entities, id, EntityId);
        }
    }
    world->entity_masks[entity] &= ~type;
}

internal void
UpdateSystems(WorldState* world)
{
    for (u32 i = 0; i < world->num_systems; ++i)
    {
        world->systems[i].Update(world, world->systems[i].entities);
    }
}

internal void*
GetComponent(WorldState* world, EntityId entity, ComponentType type)
{
    u64 index = Log2(type);
    ComponentInfo info = world->component_infos[index];
    return ((u8*)world->components[index].data) + (entity * info.size);
}