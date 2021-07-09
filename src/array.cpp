struct Array
{
    sizet size;
    sizet count;
#ifdef GAME_DEBUG
    sizet elem_size;
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

#ifdef GAME_DEBUG

#define DebugCheckArray(array, elem_size) \
    _CheckArray(array, elem_size);

#else

#define DebugCheckArray(array, elem_size)

#endif

internal Array* 
Resize(Array* array, sizet header, sizet size)
{
    Array* result = (Array*)Reallocate(array, header + size);
    Assert(result);
    return result;
}

internal void
_CheckArray(Array** array_inout, sizet elem_size)
{
    Array* array = *array_inout;
    Assert(array && array->elem_size == elem_size);
}


internal void
_ResizeArray(Array** array_inout, sizet header, sizet elem_size)
{
    DebugCheckArray(array_inout, elem_size);
    Array* array = *array_inout;
    Assert(array);

    sizet new_size = 2*array->size;
    array = Resize(array, header, new_size * elem_size);
    array->size = new_size;

    *array_inout = array;
}

internal Array* 
_CreateArray(sizet elem_count, sizet header, sizet elem_size)
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
_ArrayAdd(Array** array_inout, sizet header, sizet elem_size)
{
    DebugCheckArray(array_inout, elem_size);
    Array* array = *array_inout;

    if (!array)
    {
        array = _CreateArray(10, header, elem_size);
    }

    if (array->count >= array->size)
    {
        sizet new_size = 2*array->size;
        array = Resize(array, header, new_size * elem_size);
        array->size = new_size;
    }
    *array_inout = array;

    void* result = (u8*)((u8*)array + header) + (array->count * elem_size);
    array->count++;

    return result;
}

internal void
_ArrayRemove(Array** array_inout, sizet header, sizet elem_size, sizet index)
{
    DebugCheckArray(array_inout, elem_size);
    Array* array = *array_inout;
    Assert(array);

    if ((array->count-1) != index)
    {
        void* dest = ((u8*)array + header) + (elem_size * index);
        void* src = ((u8*)array + header) + (elem_size * (index + 1));
        sizet size = (array->count - index) * elem_size;
        MemCopy(dest, src, size);
    }

    array->count--;
    *array_inout = array;
}

internal void*
_ArrayGet(Array** array_inout, sizet header, sizet elem_size, sizet index)
{
    DebugCheckArray(array_inout, elem_size);
    Array* array = *array_inout;
    Assert(index < array->count && index >= 0);

    return (u8*)((u8*)array + header) + (index * elem_size);
}

internal void
ArrayFree(Array** array_inout)
{
    Assert(*array_inout);
    Free(*array_inout);
    *array_inout = 0;
}

