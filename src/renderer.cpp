enum TextAlign
{
    TEXT_ALIGN_MIDDLE,
    TEXT_ALIGN_LEFT,
};


internal vec2
text_box_size(DebugState* debug, char* text)
{
    vec2 out = {};
    while(*text)
    {
        if (*text >= 32 && *text <= 128)
        {
            i32 index = *text-32;
            stbtt_bakedchar* b = debug->char_metrics + index;
            out.x += b->xadvance;
        }
        text++;
    }
    out.y = debug->font_size;

    return out;
}


internal void
draw_debug_text(Renderer* ren,
                DebugState* debug,
                char* text,
                vec2 position,
                TextAlign align = TEXT_ALIGN_LEFT,
                vec4 color = {1.0f, 1.0f, 1.0f, 1.0f})
{
    if (align == TEXT_ALIGN_MIDDLE)
    {
        vec2 text_size = text_box_size(debug, text);
        position.x -= text_size.x / 2.0f;
    }


    while(*text)
    {
        if (*text >= 32 && *text <= 128)
        {
            #define IFLOOR(x) ((int) floor(x))
            stbtt_aligned_quad q;
            i32 index = *text-32;
            stbtt_bakedchar* b = debug->char_metrics + index;

            float d3d_bias = 0.0f;
            f32 ipw = 1.0f / debug->font_image.width;
            f32 iph = 1.0f / debug->font_image.height;
            i32 round_x = IFLOOR((position.x + b->xoff) + 0.5f);
            i32 round_y = IFLOOR((position.y - b->yoff) + 0.5f);

            q.x0 = round_x + d3d_bias;
            q.y1 = round_y + d3d_bias;
            q.x1 = round_x + b->x1 - b->x0 + d3d_bias;
            q.y0 = round_y - b->y1 + b->y0 + d3d_bias;
            q.s0 = b->x0 * ipw;
            q.t0 = b->y0 * iph;
            q.s1 = b->x1 * ipw;
            q.t1 = b->y1 * iph;

            position.x += b->xadvance;

            vec2 rectangle_vertices[] =
            {
                {q.x0, q.y0},
                {q.x0, q.y1},
                {q.x1, q.y1},
                {q.x0, q.y0},
                {q.x1, q.y1},
                {q.x1, q.y0},
            };

            vec2 uv_coords[] =
            {
                {q.s0, q.t1},
                {q.s0, q.t0},
                {q.s1, q.t0},
                {q.s0, q.t1},
                {q.s1, q.t0},
                {q.s1, q.t1},
            };

            for (sizet i = 0; i < ARRAY_COUNT(rectangle_vertices); ++i)
            {
                ren->vertices_2D[ren->vertex_count_2D].position = rectangle_vertices[i];
                ren->vertices_2D[ren->vertex_count_2D].uv = uv_coords[i];
                ren->vertices_2D[ren->vertex_count_2D].color = color;
                // NOTE: this coresponds to sampler2D array index in shader
                ren->vertices_2D[ren->vertex_count_2D].texture_id = 0;
                ren->vertex_count_2D++;
            }
        }
        text++;
    }
}


internal void
draw_quad(Renderer* ren, vec2 position, vec2 scale, vec4 color)
{
    vec2 rectangle_vertices[] =
    {
        {-0.5f, -0.5f},
        {-0.5f,  0.5f},
        { 0.5f,  0.5f},

        {-0.5f, -0.5f},
        { 0.5f, -0.5f},
        { 0.5f,  0.5f},
    };

    position = position + (scale/2);


    VertexData2D* vertex = ren->vertices_2D + ren->vertex_count_2D;
    for (sizet i = 0; i < ARRAY_COUNT(rectangle_vertices); ++i)
    {
        vertex->position = rectangle_vertices[i] * scale + position;
        vertex->color = color;
        vertex->uv = {};
        vertex->texture_id = -1.0f;

        ++vertex;
    }
    ren->vertex_count_2D += ARRAY_COUNT(rectangle_vertices);
}

internal void
draw_textured_quad(Renderer* ren, u32 slot, vec2 position, vec2 scale, vec4 color)
{
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

    position = position + (scale/2);


    VertexData2D* vertex = ren->vertices_2D + ren->vertex_count_2D;
    for (sizet i = 0; i < ARRAY_COUNT(rectangle_vertices); ++i)
    {
        vertex->position = rectangle_vertices[i] * scale + position;
        vertex->color = color;
        vertex->uv = uvs[i];
        vertex->texture_id = (f32)slot;

        ++vertex;
    }
    ren->vertex_count_2D += ARRAY_COUNT(rectangle_vertices);
}

#if 0
vec3 cube_vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

internal void
draw_cube(Renderer* ren, vec3 position, vec3 scale, vec4 color)
{
    sizet vertex_count = ARRAY_COUNT(cube_vertices);
    for (sizet i = 0; i < vertex_count; i+=2)
    {
        ren->vertices_start[ren->vertex_count].position = cube_vertices[i];
        ren->vertices_start[ren->vertex_count].normal = cube_vertices[i+1];
        ren->vertex_count++;
    }
}
#endif 


