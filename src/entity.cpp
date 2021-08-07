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
    
    world->query_components_map = CreateHashMap(512, HashUInt64, Array*);
}


internal void
AddComponent(WorldState* world, EntityId entity, ComponentType type)
{
    if ((world->entity_masks[entity] & type) == type)
    {
        u64 index = Log2(type);
        ComponentInfo info = world->component_infos[index];
        Print("Entity %zu already has component %s!", entity, info.name);
    }
    world->entity_masks[entity] |= type;
    
    Array** entities = HashMapGet(world->query_components_map, &world->entity_masks[entity], Array*);
    if (!entities)
    {
        entities = HashMapPut(world->query_components_map, &world->entity_masks[entity], Array*);
    }
    
    EntityId* id = ArrayAdd(*entities, EntityId);
    *id = entity;
}

internal void*
GetComponent(WorldState* world, EntityId entity, ComponentType type)
{
    u64 index = Log2(type);
    ComponentInfo info = world->component_infos[index];
    return ((u8*)world->components[index].data) + (entity * info.size);
}