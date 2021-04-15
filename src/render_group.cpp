#define PushRenderEntry(group, type) \
(type *)push_render_entry(group, sizeof(type), RENDER_ENTRY_##type) \


internal RenderGroup* 
allocate_render_group(MemoryArena* arena, u32 size, Renderer* ren, Assets* assets)
{
    RenderGroup* out = PushMemory(arena, RenderGroup);
    out->push_buffer_base = PushMemory(arena, u8, size);
    out->push_buffer_max_size = size;
    out->push_buffer_size = 0;

    out->assets = assets;
    out->renderer = ren;

    return out;
}

internal void
init_render_group(RenderGroup* group, mat4 projection)
{
    group->projection = projection;
}

internal void*
push_render_entry(RenderGroup* group, u32 size, RenderEntryType type)
{
    RenderEntryHeader* header = 0;

    if ((group->push_buffer_size + size) < group->push_buffer_max_size)
    {
        header = (RenderEntryHeader*)(group->push_buffer_base + group->push_buffer_size);
        header->entry_type = type;
        group->push_buffer_size += (size + sizeof(*header));
    }
    else
    {
       ASSERT(false);
    }

    return ((u8*)header) + sizeof(*header);
}

internal void
push_rect(RenderGroup* group, vec2 position, vec2 size, vec4 color)
{
    Quad* quad = PushRenderEntry(group, Quad);

    quad->position = position;
    quad->size = size;
    quad->color = color;
}

internal void
push_textured_rect(RenderGroup* group, ImageHandle image, vec2 position, vec2 size, vec4 color)
{
    TexturedQuad* quad = PushRenderEntry(group, TexturedQuad);

    quad->position = position;
    quad->size = size;
    quad->color = color;
    quad->image = image;
}

internal void
process_render_group(RenderGroup* group)
{
}
