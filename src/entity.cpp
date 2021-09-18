internal EntityID
AddEntity(WorldState* world)
{
    return (EntityID)world->entity_count++;
}

internal Entity*
GetEntity(WorldState* world, EntityID id)
{
    return &world->entities[id];
}

internal void
AddComponent(WorldState* world, EntityID id, u32 component_flag)
{
    Entity* entity = GetEntity(world, id);
    Assert((entity->flags & component_flag) != component_flag);
    
    entity->flags |= component_flag;
}