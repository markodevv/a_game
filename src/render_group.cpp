#define PushRenderEntry(group, type) \
(type *)push_render_entry(group, sizeof(type), RENDER_ENTRY_##type) \


internal RenderGroup 
render_group_begin(RenderSetup* render_setup, Renderer* ren, Assets* assets)
{
    RenderGroup render_group = {};

    render_group.current_setup = render_setup;
    render_group.assets = assets;
    render_group.renderer = ren;

    ren->assets = assets;

    return render_group;
}

internal void
add_image_to_setup(RenderSetup* render_setup, ImageHandle image_handle)
{
    for (u32 i = 0; i < render_setup->num_images; ++i)
    {
        if (render_setup->image_handles[i] == image_handle)
            return;
    }
    ASSERT(render_setup->num_images < 32);
    render_setup->image_handles[render_setup->num_images] = image_handle;
    ++render_setup->num_images;
}

internal void*
push_render_entry(RenderGroup* group, u32 size, RenderEntryType type)
{
    RenderEntryHeader* header = 0;

    if ((group->renderer->push_buffer_size + size) < group->renderer->push_buffer_max_size)
    {
        header = (RenderEntryHeader*)(group->renderer->push_buffer_base +  
                                      group->renderer->push_buffer_size);
        header->entry_type = type;
        group->renderer->push_buffer_size += (size + sizeof(*header));
    }
    else
    {
       ASSERT(false);
    }

    group->current_quads = 0;

    return ((u8*)header) + sizeof(*header);
}

internal TexturedQuadsEntry*
get_current_quads(RenderGroup* group, u32 num_quads)
{
    TexturedQuadsEntry* quads = group->current_quads;
    if (!quads)
    {
        quads = PushRenderEntry(group, TexturedQuadsEntry);
        *quads = {};
        quads->vertex_offset = group->renderer->vertex_count_2D;
        quads->render_setup = group->current_setup;
        group->current_quads = quads;
    }
    quads->num_quads += num_quads;

    return quads;
}

internal void
push_quad(RenderGroup* group, vec2 position, vec2 size, vec4 color)
{
    TexturedQuadsEntry* quads = get_current_quads(group, 1);
    Renderer* ren = group->renderer;

    vec2 rectangle_vertices[] =
    {
        {-0.5f, -0.5f},
        {-0.5f,  0.5f},
        { 0.5f,  0.5f},

        {-0.5f, -0.5f},
        { 0.5f, -0.5f},
        { 0.5f,  0.5f},
    };

    position = position + (size/2);


    VertexData2D* vertex = ren->vertices_2D + ren->vertex_count_2D;
    for (sizet i = 0; i < 6; ++i)
    {
        vertex->position = rectangle_vertices[i] * size + position;
        vertex->color = color;
        vertex->uv = {};
        vertex->texture_slot = 0;
        ++vertex;
    }
    ren->vertex_count_2D += 6;
}

internal void
push_quad(RenderGroup* group, ImageHandle image_handle, vec2 position, vec2 size, vec4 color)
{
    TexturedQuadsEntry* quads = get_current_quads(group, 1);
    Renderer* ren = group->renderer;

    add_image_to_setup(group->current_setup, image_handle);
    Image* image = get_loaded_image(group->assets, image_handle);

    vec2 rectangle_vertices[] =
    {
        {-0.5f, -0.5f},
        {-0.5f,  0.5f},
        { 0.5f,  0.5f},

        {-0.5f, -0.5f},
        { 0.5f, -0.5f},
        { 0.5f,  0.5f},
    };

    vec2 uvs[] = {
        { 0.0f, 0.0f},
        { 0.0f, 1.0f},
        { 1.0f, 1.0f},

        { 0.0f, 0.0f},
        { 1.0f, 0.0f},
        { 1.0f, 1.0f},
    };

    position = position + (size/2);



    VertexData2D* vertex = ren->vertices_2D + ren->vertex_count_2D;
    for (sizet i = 0; i < 6; ++i)
    {
        vertex->position = rectangle_vertices[i] * size + position;
        vertex->color = color;
        vertex->uv = uvs[i];
        vertex->texture_slot = (f32)image->slot;

        ++vertex;
    }
    ren->vertex_count_2D += 6;
}

internal void
push_quad(RenderGroup* group, 
          ImageHandle image_handle,
          vec2 positions[6], 
          vec2 uvs[6],
          vec4 color)
{
    Renderer* ren = group->renderer;
    TexturedQuadsEntry* quads = get_current_quads(group, 1);
    add_image_to_setup(group->current_setup, image_handle);

    Image* image = get_loaded_image(group->assets, image_handle);

    VertexData2D* vertex = ren->vertices_2D + ren->vertex_count_2D;

    for (sizet i = 0; i < 6; ++i)
    {
        vertex->position = positions[i];
        vertex->color = color;
        vertex->uv = uvs[i];
        vertex->texture_slot = (f32)image->slot;

        ++vertex;
    }
    ren->vertex_count_2D += 6;
}



