#define PushRenderEntry(group, type, sort_key) \
(type *)PushRenderEntry_(group, sizeof(type), RENDER_ENTRY_##type, sort_key) \


internal RenderGroup*
CreateRenderGroup(MemoryArena* arena, mat4 projection, Camera camera, Renderer* ren, Assets* assets)
{
    RenderGroup* render_group = &ren->render_groups[ren->render_group_count++];
    
    render_group->setup.projection = projection;
    render_group->assets = assets;
    render_group->setup.camera = camera;
    
    render_group->push_buffer_capacity = Megabytes(10);
    render_group->push_buffer_base = PushMemory(arena, u8, render_group->push_buffer_capacity);
    render_group->push_buffer_size = 0;
    
    return render_group;
}


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
         Layer layer,
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
             Layer layer,
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

internal f32
GetTextPixelWidth(Font* font, char* text)
{
    f32 out = 0;
    
    while(*text)
    {
        u32 index = *text-32;
        out += font->char_metrics[index].xadvance;
        
        text++;
    }
    
    return out;
}


internal void
DrawText(RenderGroup* render_group,
         Font* font,
         char* text,
         vec2 position,
         Layer layer,
         TextAlign align = TEXT_ALIGN_LEFT,
         Color color = {255, 255, 255, 254})
{
    if (align == TEXT_ALIGN_MIDDLE)
    {
        f32 text_width = GetTextPixelWidth(font, text);
        position.x -= (text_width / 2.0f);
    }
    
    while(*text)
    {
        if (*text >= 32 && *text <= 128)
        {
            i32 index = *text-32;
            CharMetric* cm = font->char_metrics + index;
            
            i32 round_x = IFloor((position.x + cm->xoff) + 0.5f);
            i32 round_y = IFloor((position.y - cm->yoff) + 0.5f);
            
            vec2 pos = V2(round_x, round_y);
            vec2 size = V2(cm->x1 - cm->x0,
                           -cm->y1 + cm->y0);
            
            PushQuad(render_group, pos, size, color, layer, font->sprite_handles[index]);
            
            position.x += cm->xadvance;
        }
        text++;
    }
}
