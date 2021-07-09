internal u32 
HashInt(u32 x) 
{
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

internal u32
HashString(char* key)
{
    u32 hash = 5381;
    i32 c;
    
    while((c = *key++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

internal MapBucket*
GetEntity(WorldState* world, EntityId key)
{
    u32 hash = HashInt(key);
    u32 index = hash % ENTITY_MAX;

    MapBucket* result;

    result = world->entity_map + index;

    return result;
}

internal MapBucket*
GetComponent(WorldState* world, char* key)
{
    u32 hash = HashString(key);
    u32 index = hash % ENTITY_MAX;

    MapBucket* result;

    result = world->entity_map + index;

    return result;
}

internal MapBucket*
EntityMapGetWIndex(WorldState* world, char* index)
{
    return world->entity_map + index;
}

#define RegisterComponent(world, type) \
    _RegisterComponent(world, #type);

internal void
_RegisterComponent(WorldState* world, char* key)
{
    MapBucket* bucket = GetComponent(world, key);
    Assert(bucket->name == 0);
    bucket->name = key;
}

#define AddComponent(world, entity, type) \
    _AddComponent(world, entity, #type);

internal void
_AddComponent(WorldState* world, EntityId id, char* key)
{
    u32 hash = HashString(key);
    u32 index = hash % ENTITY_MAX;

    MapBucket* entity = GetEntity(world, id);
    EntityId* component = ArrayAdd(&bucket->components, EntityId);
    *component = GetComponent(&world->entity_map, key)->;
}

internal EntityId
EcsNewId(WorldState* world)
{
    EntityId result = 0;
    local_persist u32 id = 100;

    Array* removed_ids = world->removed_ids;
    if (world->removed_ids->count)
    {
        result = *ArrayGet(&removed_ids, EntityId, (removed_ids->count - 1));
        ArrayRemove(&removed_ids, EntityId, removed_ids->count);
    }
    else
    {
        // If theres already component in map skip it.
        while (GetEntity(world, id)->name)
        {
            id++;
        }
        result = id;
    }

    return result;
}

internal void
EcsInit(WorldState* world)
{
    world->entity_map = (Record*)Allocate(sizeof(Record) * ENTITY_MAX);
    world->entity_index_map = (MapData*)Allocate(sizeof(MapData) * ENTITY_MAX);
    world->removed_ids = CreateArray(10, EntityId);
    world->archetypes = CreateArray(10, ArcheType);

    RegisterComponent(world, Transform);

    EntityId player = EcsNewId(world);
    AddComponent(world, player, Transform);
}
