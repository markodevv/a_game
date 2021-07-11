#define RegisterComponent(world, type) \
    _RegisterComponent(world, #type, sizeof(type));

internal void
_RegisterComponent(WorldState* world, char* key, u32 size)
{
    sizet component_id = HashString(key);
    ComponentInfo* info = HashMapGet(world->component_info_map, &component_id, ComponentInfo);
    info->name = key;
    info->size = size;
    info->id = component_id;
}

#define AddComponent(world, entity, component) \
    _AddComponent(world, entity, #component);

internal HashMapKey
HashFromComponentIds(Array* components)
{
    HashMapKey result = 0;
    for (u32 i = 0; i < components->count; ++i)
    {
        EntityId component_id = *ArrayGet(&components, EntityId, i);
        result += (component_id);
    }

    return result;
}

internal ArcheType*
GetArcheType(WorldState* world, Array* component_ids)
{
    HashMapKey hash_key = HashFromComponentIds(component_ids);
    ArcheType* archetype = HashMapGet(world->archetype_map, &hash_key, ArcheType);

    if (!archetype->components)
    {
        archetype->components = (Array**)Allocate(sizeof(Array**) * archetype->count)
        for (u32 i = 0; i < component_ids->count; ++i)
        {
            EntityId* id = ArrayAdd(&archetype->type, EntityId);
            *id = *ArrayGet(&component_ids, EntityId, i);
            ComponentInfo* comp_info = HashMapGet(world->component_info_map, id, ComponentInfo);

            archetype->components[i] = _CreateArray(10, sizeof(Array), comp_info->size);
        }
    }


    return archetype;
}

// TODO ////////////////
// Components on entity are not necesserily gonna be added in same order, so using
// that array for archetype type is a bad idea. 

internal u32
AddEntryArcheType(WorldState* world, ArcheType* archetype)
{
    u32 result = 0;
    for (u32 i = 0; i < archetype->type->count; ++i)
    {
        EntityId comp_id = *ArrayGet(&archetype->type, EntityId, i);
        ComponentInfo* comp_info = HashMapGet(world->component_info_map, &comp_id, ComponentInfo);
        _ArrayAdd(&archetype->components[i], sizeof(Array), comp_info->size);
    }
    result = (archetype->components[0]->count - 1);

    return result;
}

internal void
_AddComponent(WorldState* world, EntityId entity, char* key)
{
    u64 component_id = HashString(key);
    Array** component_array = HashMapGet(world->entity_map, &entity, Array*);

    if ((*component_array))
    {
        for (u32 i = 0; i < (*component_array)->count; ++i)
        {
            EntityId id = *ArrayGet(component_array, EntityId, i)
            if (id == component_id)
            {
                Print("Entity already has this component");
                return;
            }
        }
    }

    Record* record = HashMapGet(world->record_map, &entity, Record);

    EntityId* component = ArrayAdd(component_array, EntityId);
    *component = component_id;

    record->archetype = GetArcheType(world, *component_array);
    record->row = AddEntryArcheType(world, record->archetype);
}

#define GetComponent(world, entity, component) \
    (component*)_GetComponent(world, entity, #component);

internal void*
_GetComponent(WorldState* world, EntityId entity, char* key)
{
    void* result = 0;
    u64 component_id = HashString(key);
    Record* record = HashMapGet(world->record_map, &entity, Record);
    ArcheType* archetype = record->archetype;
    Array* type = record->archetype->type;

    for (u32 i = 0; i < type->count; ++i)
    {
        EntityId id = *ArrayGet(&type, EntityId, i);
        if (component_id == id)
        {
            ComponentInfo* comp_info = HashMapGet(world->component_info_map, &component_id, ComponentInfo);
            result = _ArrayGet(&archetype->components[i], sizeof(Array), comp_info->size, record->row);
            break;
        }
    }

    return result;
}

internal EntityId
NewEntity(WorldState* world)
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
        id++;
        result = id;
    }

    return result;
}


internal void
EcsInit(WorldState* world)
{
    world->entity_map = CreateHashMap(5120, HashInt, Array*);
    world->archetype_map = CreateHashMap(512, HashInt, ArcheType);
    world->record_map = CreateHashMap(512, HashInt, Record);
    world->component_info_map = CreateHashMap(512, HashInt, ComponentInfo);
    world->removed_ids = CreateArray(10, EntityId);

}
