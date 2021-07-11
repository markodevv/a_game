struct Array
{
    u32 size;
    u32 count;
#ifdef GAME_DEBUG
    u32 elem_size;
#endif
};

#define ResizeArray(array, type) \
    _ResizeArray(array, sizeof(Array), sizeof(type));

#define CreateArray(size, type) \
    _CreateArray(size, sizeof(Array), sizeof(type));

#define ArrayAdd(array, type) \
    (type*)_ArrayAdd(array, sizeof(Array), sizeof(type));

#define ArrayRemove(array, type, index) \
    _ArrayRemove(array, sizeof(Array), sizeof(type), index);

#define ArrayGet(array, type, index) \
    (type*)_ArrayGet(array, sizeof(Array), sizeof(type), index);

#define ArraySet(array, type, index, data) \
    (type*)_ArraySet(array, sizeof(Array), sizeof(type), index, data);

#define ArrayFirst(array, type) \
    (type*)_ArrayFirst(array, sizeof(Array), sizeof(type));

#ifdef GAME_DEBUG

#define DebugCheckArray(array, elem_size) \
    _CheckArray(array, elem_size);

#else

#define DebugCheckArray(array, elem_size)

#endif


internal Array* 
Resize(Array* array, u32 header, u32 size)
{
    Array* result = (Array*)Reallocate(array, header + size);
    Assert(result);
    return result;
}

internal void
_CheckArray(Array** array_inout, u32 elem_size)
{
    Array* array = *array_inout;
    Assert(array && array->elem_size == elem_size);
}


internal void
_ResizeArray(Array** array_inout, u32 header, u32 elem_size)
{
    DebugCheckArray(array_inout, elem_size);
    Array* array = *array_inout;
    Assert(array);

    u32 new_size = 2*array->size;
    array = Resize(array, header, new_size * elem_size);
    array->size = new_size;

    *array_inout = array;
}

internal Array* 
_CreateArray(u32 elem_count, u32 header, u32 elem_size)
{
    Array* array = (Array*)Allocate(header + elem_size * elem_count);
    array->size = elem_count;
    array->count = 0;

#ifdef GAME_DEBUG
    array->elem_size = elem_size;
#endif

    return array;
}


internal void*
_ArrayAdd(Array** array_inout, u32 header, u32 elem_size)
{
    Array* array = *array_inout;

    if (!array)
    {
        array = _CreateArray(10, header, elem_size);
    }

    if (array->count >= array->size)
    {
        u32 new_size = 2*array->size;
        array = Resize(array, header, new_size * elem_size);
        array->size = new_size;
    }
    *array_inout = array;

    DebugCheckArray(array_inout, elem_size);

    void* result = (u8*)((u8*)array + header) + (array->count * elem_size);
    array->count++;

    return result;
}

internal void*
_ArrayFirst(Array** array_inout, u32 header, u32 elem_size)
{
    Array* array = *array_inout;
    return ((u8*)array + header);
}

internal void
_ArrayRemove(Array** array_inout, u32 header, u32 elem_size, u32 index)
{
    DebugCheckArray(array_inout, elem_size);
    Array* array = *array_inout;
    Assert(array);

    if ((array->count-1) != index)
    {
        void* dest = ((u8*)array + header) + (elem_size * index);
        void* src = ((u8*)array + header) + (elem_size * (index + 1));
        u32 size = (array->count - index) * elem_size;
        MemCopy(dest, src, size);
    }

    array->count--;
    *array_inout = array;
}

internal void*
_ArrayGet(Array** array_inout, u32 header, u32 elem_size, u32 index)
{
    void* result = 0;
    if (*array_inout)
    {
        DebugCheckArray(array_inout, elem_size);
        Array* array = *array_inout;
        Assert(index < array->count && index >= 0);
        result = (u8*)((u8*)array + header) + (index * elem_size);
    }

    return result;
}

internal void
_ArraySet(Array** array_inout, u32 header, u32 elem_size, u32 index, const void* data)
{
    DebugCheckArray(array_inout, elem_size);
    Array* array = *array_inout;
    Assert(index < array->count && index >= 0);

    void* dest = (u8*)((u8*)array + header) + (index * elem_size);

    MemCopy(dest, data, elem_size);
}

internal void
ArrayFree(Array** array_inout)
{
    Assert(*array_inout);
    Free(*array_inout);
    *array_inout = 0;
}

