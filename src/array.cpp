struct Array
{
    u32 size;
    u32 count;
    u32 elem_size;
};

#define ResizeArray(array, type) \
_ResizeArray(&array, sizeof(Array), sizeof(type));

#define CreateArray(size, type) \
_CreateArray(size, sizeof(Array), sizeof(type));

#define ArrayAdd(array, type) \
((type*)_ArrayAdd(&array, sizeof(Array), sizeof(type)));

#define ArrayRemove(array, type, index) \
_ArrayRemove(&array, sizeof(Array), sizeof(type), index);

#define ArrayGet(array, index, type) \
((type*)_ArrayGet(&array, sizeof(Array), sizeof(type), index));

#define ArrayClearMember(array, type, index) \
(type*)_ArrayClearMember(&array, sizeof(Array), sizeof(type), index);

#define ArrayFirst(array, type) \
(type*)_ArrayFirst(array, sizeof(Array), sizeof(type));

#define ArrayCopy(array, type) \
_ArrayCopy(&array, sizeof(Array), sizeof(type));

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
    Array* array = (Array*)Allocate(u8, header + elem_size * elem_count);
    array->size = elem_count;
    array->count = 0;
    
    array->elem_size = elem_size;
    
    return array;
}

internal Array*
_ArrayCopy(Array* array_src, u32 header, u32 elem_size)
{
    Array* result = 0;
    u32 size = (elem_size * (array_src->size));
    result = (Array*)Allocate(u8, header + size);
    MemCopy(result, array_src, size);
    
    return result;
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
_ArrayClearMember(Array** array_inout, u32 header, u32 elem_size, u32 index)
{
    DebugCheckArray(array_inout, elem_size);
    Array* array = *array_inout;
    Assert(index < array->count && index >= 0);
    
    void* dest = (u8*)((u8*)array + header) + (index * elem_size);
    
    MemClear(dest, elem_size);
}

internal void
ArrayFree(Array** array_inout)
{
    Assert(*array_inout);
    Free(*array_inout);
    *array_inout = 0;
}

typedef void ArraySortProc(void* data, u32 count);


internal void 
SortU64(void* data, u32 count)
{
    u64* array = (u64*)data;
    for (u32 i = 1; i < count; ++i)
    {
        u64 sort_elem = array[i];
        
        i32 j = i - 1;
        while(j >= 0 && array[j] > sort_elem)
        {
            u64* to_set = (((u64*)data) + j + 1);
            array[j + 1] = array[j];
            --j;
        }
        array[j + 1] = sort_elem;
    }
}

#define ArraySort(array, Sort) \
_ArraySort(array, Sort, sizeof(Array));

internal void
_ArraySort(Array** array_inout, ArraySortProc* Sort, u32 header)
{
    Array* arr = *array_inout;
    Sort(((u8*)arr + header), arr->count);
}

