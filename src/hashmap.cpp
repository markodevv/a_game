typedef u64 HashMapKey;

typedef HashMapKey HashFunctionProc(const void* to_hash);
typedef HashMapKey CompareKeyProc(const void* key1, const void* key2);


struct HashEntry
{
    Array* keys;
    Array* values;
};

struct HashMap
{
    u32 size;
    u32 count;
    u32 key_size;
    
    HashEntry* entries;
    
    HashFunctionProc* Hash;
    CompareKeyProc* CompareKey;
#ifdef GAME_DEBUG
    u32 elem_size;
#endif
};


#define HashMapGet(map, key, type) \
((type*)_HashMapGet(&map, key, sizeof(*key), sizeof(type)));

#define HashMapPut(map, key, type) \
((type*)_HashMapPut(&map, key, sizeof(*key), sizeof(type)));

#define CreateHashMap(size, hash, type) \
_CreateHashMap(size, sizeof(type), hash);


internal b8
CompareU32(const void* key1, void* key2)
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
HashUInt64(const void* data) 
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
    u64 hash = 5381;
    i32 c;
    
    while((c = *key++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}



internal HashMap
_CreateHashMap(u32 elem_count, 
               u32 elem_size, 
               HashFunctionProc hash_function)
{
    HashMap result = {};
    result.entries = Allocate(HashEntry, elem_count);
    result.size = elem_count;
    result.Hash = hash_function;
    result.count = 0;
    
#ifdef GAME_DEBUG
    result.elem_size = elem_size;
#endif
    
    return result;
}

internal void*
_HashMapPut(HashMap* map, const void* key, u32 key_size, u32 value_size)
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
            void* key_cmp = _ArrayGet(&entry->keys, sizeof(Array), key_size, i);
            if (MemCompare(key, key_cmp, key_size))
            {
                Print("Hash entry with key [%s] already exists!", (char*)key);
                Assert(false);
            }
        }
    }
    
    void* to_add_key = _ArrayAdd(&entry->keys, sizeof(Array), key_size);
    MemCopy(to_add_key, key, key_size);
    result = _ArrayAdd(&entry->values, sizeof(Array), value_size);
    
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
            void* key_cmp = _ArrayGet(&entry->keys, sizeof(Array), key_size, i);
            if (MemCompare(key, key_cmp, key_size))
            {
                result = _ArrayGet(&entry->values, sizeof(Array), value_size, i);
                break;
            }
        }
    }
    
    return result;
}

#define HashMapSet(map, key, value) \
_HashMapSet(map, key, sizeof(*key), value, sizeof(*value));


internal void
_HashMapSet(HashMap map, const void* key, u32 key_size, void* value, u32 value_size)
{
    void* to_set = _HashMapGet(&map, key, key_size, value_size);
    MemCopy(to_set, value, value_size);
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






