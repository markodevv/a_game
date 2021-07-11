typedef u64 HashMapKey;

typedef HashMapKey HashFunctionProc(const void* to_hash);


struct HashEntry
{
    Array* keys;
    Array* values;
};

struct HashMap
{
    u32 size;
    u32 count;
    HashEntry* entries;
    HashFunctionProc* Hash;
#ifdef GAME_DEBUG
    u32 elem_size;
#endif
};

#define HashMapGet(map, key, type) \
    (type*)_HashMapGet(map, key, sizeof(*key), sizeof(type));

#define CreateHashMap(size, hash, type) \
    _CreateHashMap(size, sizeof(type), hash);


internal b8
CompareInt(const void* key1, void* key2)
{
    u32 int1 = *((u32*)key1);
    u32 int2 = *((u32*)key2);

    return (int1 == int2);
}

internal sizet
CStringLiteralLen(char* str)
{
    return sizeof(str) + 1;
}

internal b8
CompareString(const void* key1, void* key2)
{
    char* s1 = (char*)key1;
    char* s2 = (char*)key2;

    for (u32 i = 0; i < CStringLiteralLen(s1); ++i)
    {
        if (s1[i] != s2[i])
        {
            return false;
        }
    }

    return true;
}

internal HashMapKey
HashInt(const void* data) 
{
    sizet x = *((sizet*)data);
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

internal HashMapKey
HashString(const void* data)
{
    char* key = (char*)data;
    sizet hash = 5381;
    i32 c;
    
    while((c = *key++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}


internal HashMap*
_CreateHashMap(u32 elem_count, u32 elem_size, HashFunctionProc hash_function)
{
    HashMap* result = (HashMap*)Allocate(sizeof(HashMap));
    result->entries = (HashEntry*)Allocate(elem_count * sizeof(HashEntry));
    result->size = elem_count;
    result->Hash = hash_function;
    result->count = 0;

#ifdef GAME_DEBUG
    result->elem_size = elem_size;
#endif

    return result;
}




internal void*
_HashMapGet(HashMap* map, const void* key, u32 key_size, u32 value_size)
{
    Assert(map->elem_size == value_size);

    void* result = 0;
    sizet hash = map->Hash(key);
    sizet index = hash % (map->size-1);

    HashEntry* entry = map->entries + index;

    Assert(entry);

    if (entry->keys)
    {
        for (u32 i = 0; i < entry->keys->count; ++i)
        {
            HashMapKey hash_cmp = *ArrayGet(&entry->keys, HashMapKey, i);
            if (hash_cmp == hash)
            {
                result = _ArrayGet(&entry->values, sizeof(Array), value_size, i);
                return result;
            }
        }
    }

    HashMapKey* to_add_key = (HashMapKey*)_ArrayAdd(&entry->keys, sizeof(Array), sizeof(HashMapKey));
    *to_add_key = hash;
    result = _ArrayAdd(&entry->values, sizeof(Array), value_size);

    return result;
}

internal void
HashMapFree(HashMap* map)
{
    for (u32 i = 0; i < map->size; ++i)
    {
        HashEntry* entry = map->entries + i;
        if (entry->keys)
        {
            ArrayFree(&entry->keys);
            ArrayFree(&entry->values);
        }
    }
    Assert(map);
    Free(map->entries);
    Free(map);
}






