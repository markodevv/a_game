global_variable vec3 cube_vertices[] = {
   {-0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f, -0.5f}, 
   { 0.5f,  0.5f, -0.5f}, 
   { 0.5f,  0.5f, -0.5f}, 
   {-0.5f,  0.5f, -0.5f}, 
   {-0.5f, -0.5f, -0.5f}, 

   {-0.5f, -0.5f,  0.5f}, 
   { 0.5f, -0.5f,  0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 
   {-0.5f,  0.5f,  0.5f}, 
   {-0.5f, -0.5f,  0.5f}, 

   {-0.5f,  0.5f,  0.5f}, 
   {-0.5f,  0.5f, -0.5f}, 
   {-0.5f, -0.5f, -0.5f}, 
   {-0.5f, -0.5f, -0.5f}, 
   {-0.5f, -0.5f,  0.5f}, 
   {-0.5f,  0.5f,  0.5f}, 

   { 0.5f,  0.5f,  0.5f}, 
   { 0.5f,  0.5f, -0.5f}, 
   { 0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f,  0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 

   {-0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f,  0.5f}, 
   { 0.5f, -0.5f,  0.5f}, 
   {-0.5f, -0.5f,  0.5f}, 
   {-0.5f, -0.5f, -0.5f}, 

   {-0.5f,  0.5f, -0.5f}, 
   { 0.5f,  0.5f, -0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 
   {-0.5f,  0.5f,  0.5f}, 
   {-0.5f,  0.5f, -0.5f}, 
};

global_variable vec3 cube_normals[] =
{
   { 0.0f,  0.0f,  -1.0f}, 
   { 0.0f,  0.0f,  -1.0f}, 
   { 0.0f,  0.0f,  -1.0f}, 
   { 0.0f,  0.0f,  -1.0f}, 
   { 0.0f,  0.0f,  -1.0f}, 
   { 0.0f,  0.0f,  -1.0f}, 

   { 0.0f,  0.0f,  1.0f}, 
   { 0.0f,  0.0f,  1.0f}, 
   { 0.0f,  0.0f,  1.0f}, 
   { 0.0f,  0.0f,  1.0f}, 
   { 0.0f,  0.0f,  1.0f}, 
   { 0.0f,  0.0f,  1.0f}, 

   { -1.0f,  0.0f,  0.0f}, 
   { -1.0f,  0.0f,  0.0f}, 
   { -1.0f,  0.0f,  0.0f}, 
   { -1.0f,  0.0f,  0.0f}, 
   { -1.0f,  0.0f,  0.0f}, 
   { -1.0f,  0.0f,  0.0f}, 

   { 1.0f,  0.0f,  0.0f}, 
   { 1.0f,  0.0f,  0.0f}, 
   { 1.0f,  0.0f,  0.0f}, 
   { 1.0f,  0.0f,  0.0f}, 
   { 1.0f,  0.0f,  0.0f}, 
   { 1.0f,  0.0f,  0.0f}, 

   { 0.0f,  -1.0f,  0.0f}, 
   { 0.0f,  -1.0f,  0.0f}, 
   { 0.0f,  -1.0f,  0.0f}, 
   { 0.0f,  -1.0f,  0.0f}, 
   { 0.0f,  -1.0f,  0.0f}, 
   { 0.0f,  -1.0f,  0.0f}, 

   { 0.0f,  1.0f,  0.0f}, 
   { 0.0f,  1.0f,  0.0f}, 
   { 0.0f,  1.0f,  0.0f}, 
   { 0.0f,  1.0f,  0.0f}, 
   { 0.0f,  1.0f,  0.0f}, 
   { 0.0f,  1.0f,  0.0f}, 
};



enum TextAlign
{
    TEXT_ALIGN_MIDDLE,
    TEXT_ALIGN_LEFT,
};

internal vec2
text_box_size(Renderer* ren, char* text)
{
    vec2 out = {};
    while(*text)
    {
        if (*text >= 32 && *text <= 128)
        {
            i32 index = *text-32;
            stbtt_bakedchar* b = ren->char_metrics + index;
            out.x += b->xadvance;
        }
        text++;
    }
    out.y = ren->font_size;

    return out;
}


internal void
draw_debug_text(Renderer* ren,
                char* text,
                vec3 position,
                TextAlign align = TEXT_ALIGN_LEFT,
                vec4 color = {1.0f, 1.0f, 1.0f, 1.0f})
{
    if (align == TEXT_ALIGN_MIDDLE)
    {
        vec2 text_size = text_box_size(ren, text);
        position.x -= text_size.x / 2.0f;
    }


    f32 z = position.z;
    while(*text)
    {
        if (*text >= 32 && *text <= 128)
        {
            #define IFLOOR(x) ((int) floor(x))
            stbtt_aligned_quad q;
            i32 index = *text-32;
            stbtt_bakedchar* b = ren->char_metrics + index;

            float d3d_bias = 0.0f;
            f32 ipw = 1.0f / ren->font_bitmap.width;
            f32 iph = 1.0f / ren->font_bitmap.height;
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

            vec3 rectangle_vertices[] =
            {
                {q.x0, q.y0, z},
                {q.x0, q.y1, z},
                {q.x1, q.y1, z},
                {q.x0, q.y0, z},
                {q.x1, q.y1, z},
                {q.x1, q.y0, z},
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
                ren->vertices_2D[ren->vertex_index_2D].position = rectangle_vertices[i];
                ren->vertices_2D[ren->vertex_index_2D].uv = uv_coords[i];
                ren->vertices_2D[ren->vertex_index_2D].color = color;
                ren->vertices_2D[ren->vertex_index_2D].texture_id = (f32)ren->font_texture_id;
                ren->vertex_index_2D++;
            }
        }
        text++;
    }
}

internal void
draw_rect(Renderer* ren, vec3 position, vec2 scale, vec4 color)
{

    vec3 rectangle_vertices[] =
    {
        {-0.5f, -0.5f, 0.0f},
        {-0.5f,  0.5f, 0.0f},
        { 0.5f,  0.5f, 0.0f},

        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.5f,  0.5f, 0.0f},
    };
    vec3 scal = V3(scale);
    position = position + (scal/2);


    for (sizet i = 0; i < ARRAY_COUNT(rectangle_vertices); ++i)
    {
        ren->vertices_2D[ren->vertex_index_2D].position =      
                                 rectangle_vertices[i] * scal + position;
        ren->vertices_2D[ren->vertex_index_2D].color = color;
        ren->vertices_2D[ren->vertex_index_2D].uv = {};
        ren->vertices_2D[ren->vertex_index_2D].texture_id = -1.0f;
        ren->vertex_index_2D++;
    }
}

internal void
draw_cube(Renderer* ren, vec3 position, vec3 scale, vec4 color)
{
    for (sizet i = 0; i < ARRAY_COUNT(cube_vertices); i++)
    {
        ren->vertices_start[ren->vertex_index].position = cube_vertices[i];
        ren->vertices_start[ren->vertex_index].normal = cube_normals[i];
        ren->vertices_start[ren->vertex_index].color = color;
        ren->vertex_index++;
    }
}


internal void
texture_test(Renderer* ren, vec2 position, vec2 scale, vec4 color)
{
    /*
    f32 x = position.x;
    f32 y = position.y;
    f32 w = scale.x;
    f32 h = scale.y;

    vec2 rectangle_vertices[] =
    {
        {-0.5f*w+x, -0.5f*h+y},
        {-0.5f*w+x,  0.5f*h+y},
        { 0.5f*w+x,  0.5f*h+y},

        {-0.5f*w+x, -0.5f*h+y},
        { 0.5f*w+x, -0.5f*h+y},
        { 0.5f*w+x,  0.5f*h+y},
    };
    vec2 uv_coords[] =
    {
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {1.0f, 0.0f},
    };
    for (sizet i = 0; i < ARRAY_COUNT(rectangle_vertices); ++i)
    {
        ren->vertices_2D[ren->vertex_index_2D].position = rectangle_vertices[i];
        ren->vertices_2D[ren->vertex_index_2D].uv = uv_coords[i];
        ren->vertices_2D[ren->vertex_index_2D].color = color;
        ren->vertices_2D[ren->vertex_index_2D].texture_id = -1;
        ren->vertex_index_2D++;
    }
    */
}
