#define PushRenderEntry(group, type, sort_key) \
(type *)PushRenderEntry_(group, sizeof(type), RENDER_ENTRY_##type, sort_key) \


internal RenderGroup*
SetupRenderGroup(MemoryArena* arena, mat4 projection, Renderer* ren, Assets* assets)
{
    RenderGroup* render_group;

    if (!ren->render_groups)
    {
        ren->render_groups = PushMemory(arena, RenderGroup);
        *ren->render_groups = {};
        render_group = ren->render_groups;
    }
    else
    {
        render_group = ren->render_groups;
        while(render_group->next)
        {
            render_group = render_group->next;
        }
        render_group->next = PushMemory(arena, RenderGroup);
        render_group = render_group->next;
        *render_group = {};
    }

    render_group->setup.projection = projection;
    render_group->assets = assets;

    render_group->push_buffer_capacity = Megabytes(10);
    render_group->push_buffer_base = PushMemory(arena, u8, render_group->push_buffer_capacity);
    render_group->push_buffer_size = 0;

    Assert(arena->temp_arena_count);

    return render_group;
}

// internal void
// AddSpriteToSetup(RenderSetup* render_setup, SpriteHandle sprite_handle)
// {
    // for (u32 i = 0; i < render_setup->num_sprites; ++i)
    // {
        // if (render_setup->sprite_handles[i] == sprite_handle)
            // return;
    // }
    // Assert(render_setup->num_sprites < 32);
    // render_setup->sprite_handles[render_setup->num_sprites] = sprite_handle;
    // ++render_setup->num_sprites;
// }


internal void 
PushBackSortElement(RenderGroup* group, u32 sort_key)
{
    SortElement* end = (SortElement*)((group->push_buffer_base + group->push_buffer_capacity) - sizeof(SortElement));
    SortElement* next_element = (end - group->sort_element_count);
    next_element->entry_offset = group->push_buffer_size;
    next_element->key = sort_key;

    ++group->sort_element_count;
    Assert((u8*)next_element > (group->push_buffer_base + group->push_buffer_size));
}

internal SortElement*
SortElementStart(RenderGroup* group)
{
    SortElement* end = (SortElement*)((group->push_buffer_base + 
                                       group->push_buffer_capacity) - 
                                       (sizeof(SortElement)));
    SortElement* start = end - (group->sort_element_count - 1);

    return start;
}

internal void*
PushRenderEntry_(RenderGroup* group, u32 size, RenderEntryType type, u32 sort_key)
{
    RenderEntryHeader* header = 0;
    void* result = 0;

    if ((group->push_buffer_size + size) < group->push_buffer_capacity)
    {
        header = (RenderEntryHeader*)(group->push_buffer_base +  
                                      group->push_buffer_size);
        header->entry_type = type;

        result = ((u8*)header) + sizeof(*header); 
        MemoryClear(result, size);


        PushBackSortElement(group, sort_key);

        group->push_buffer_size += (size + sizeof(*header));
    }
    else
    {
       Assert(false);
    }


    return result;
}



internal void
PushQuad(RenderGroup* group, 
          vec2 position, 
          vec2 size, 
          Color color, 
          f32 layer,
          SpriteHandle sprite_handle = 0,
          ShaderId shader_id = SHADER_ID_NORMAL)
{
    u32 key = shader_id;

    if (color.a != 255)
    {
        key += 100 + layer;
    }

    QuadEntry* entry = PushRenderEntry(group, QuadEntry, key);



    entry->position = V3(position, layer);
    entry->size = size;
    entry->color = color;
    entry->sprite_handle = sprite_handle;
    entry->shader_id = shader_id;
}




internal void
PushTriangle(RenderGroup* group, 
              vec2 p1, 
              vec2 p2, 
              vec2 p3, 
              Color color, 
              f32 layer,
              SpriteHandle sprite_handle = 0,
              ShaderId shader_id = SHADER_ID_NORMAL)
{
    u32 key = shader_id;

    if (color.a != 255)
    {
        key += 50 + layer;
    }

    TriangleEntry* entry = PushRenderEntry(group, TriangleEntry, key);

    entry->points[0] = V3(p1, layer);
    entry->points[1] = V3(p2, layer);
    entry->points[2] = V3(p3, layer);
    entry->color = color;
    entry->sprite_handle = sprite_handle;
    entry->shader_id = shader_id;
}
