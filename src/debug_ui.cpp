global_variable Color bg_main_color = {43, 46, 74, 255};
global_variable Color faint_bg_color = {25, 25, 45, 255};
global_variable Color faint_red_color = {144, 55, 33, 255};
global_variable Color faint_redblue_color = {83, 53, 74, 255};
global_variable Color red_color = {232, 69, 69, 255};

#define PADDING 4.0f
#define MAX_NAME_WIDTH 120

// WARNING: Macro madness bellow, don't look at it for too long!!

#define DebugFloat32Editbox(debug, var, name) \
    AnyTypeMultibox(debug, var, (sizeof(*var)/sizeof(f32)), sizeof(f32), name, f32, false)

#define DebugFloat64Editbox(debug, var, name) \
    AnyTypeMultibox(debug, var, (sizeof(*var)/sizeof(f64)), sizeof(f64), name, f64, false)

#define DebugInt32Editbox(debug, var, name) \
    AnyTypeMultibox(debug, var, (sizeof(*var)/sizeof(i32)), sizeof(i32), name, i32, true)

#define DebugInt8Editbox(debug, var, name) \
    AnyTypeMultibox(debug, var, (sizeof(*var)/sizeof(i8)), sizeof(i8), name, i8, true)

#define DebugUInt8Editbox(debug, var, name) \
    AnyTypeMultibox(debug, var, (sizeof(*var)/sizeof(u8)), sizeof(u8), name, u8, true)


#define AnyTypeMultibox(debug, v, num_components, component_size, var_name, type, is_int) \
{ \
    f32 total_width = (debug->window->size.x - MAX_NAME_WIDTH - 2*PADDING) - ((num_components-1) * PADDING); \
    type* iterator = (type*)v; \
    DrawEditbox(debug, iterator, (*iterator), var_name, V2(total_width/num_components, 20), type, is_int) \
 \
    for (u32 i = 1; i < num_components; ++i) \
    { \
        debug_cursor_sameline(debug); \
        iterator++; \
        DrawEditbox(debug, iterator, (*iterator), 0, V2(total_width/num_components, 20), type, is_int) \
    } \
} \

#define DrawEditbox(debug, pointer, value, var_name, size, type, is_int) \
{ \
    RenderGroup* group = debug->render_group; \
    if (var_name) \
    { \
        debug_cursor_newline(debug, size.x + MAX_NAME_WIDTH, size.y); \
    } \
    else \
    { \
        debug_cursor_newline(debug, size.x, size.y); \
    } \
    vec2 pos = debug->draw_cursor; \
    Color color = bg_main_color; \
    b8 is_editing_box = false; \
    if (var_name) \
    { \
        if (name_is_too_long(&debug->font, var_name)) \
        { \
            char* temp_name = string_copy(&debug->arena, var_name); \
            truncate_var_name(temp_name); \
            debug_text(debug->render_group,  \
                       &debug->font, temp_name,  \
                       V2(pos.x, pos.y + (size.y - debug->font.font_size)),  \
                       debug->window->layer + LAYER_FRONT); \
        } \
        else \
        { \
            debug_text(debug->render_group,  \
                       &debug->font, var_name,  \
                       V2(pos.x, pos.y + (size.y - debug->font.font_size)),  \
                       debug->window->layer + LAYER_FRONT); \
        } \
    } \
    pos.x += MAX_NAME_WIDTH; \
 \
 \
    char* text = get_text_to_draw(&debug->arena, value); \
 \
    if (is_interacting(debug, pointer)) \
    { \
        if (is_first_interaction(debug, pointer)) \
        { \
            fill_text_buffer(debug, value); \
        } \
 \
        debug->editbox_value_to_set = iterator; \
        debug->editbox_value_to_set_size = sizeof(type); \
        debug->editbox_value_is_int = is_int; \
 \
        is_editing_box = true; \
        debug_text(debug->render_group, &debug->font, debug->text_input_buffer, V2(pos.x, pos.y + (size.y - debug->font.font_size)), debug->window->layer + LAYER_FRONT); \
        debug->text_input_buffer[debug->text_insert_index] = '\0'; \
 \
        f32 cursor_x = pos.x + get_text_width(&debug->font, debug->text_input_buffer); \
        push_quad(debug->render_group, V2(cursor_x, pos.y), V2(3, size.y), faint_red_color, debug->window->layer + LAYER_FRONT); \
    } \
    else \
    { \
        if (is_hot(debug, pointer)) \
        { \
            color.r *= 2; \
            color.g *= 2; \
            color.b *= 2; \
        } \
        debug_text(debug->render_group,  \
                   &debug->font, \
                   text,  \
                   V2(pos.x + (size.x / 2),  \
                      pos.y + (size.y - debug->font.font_size)), \
                   debug->window->layer + LAYER_FRONT, \
                   TEXT_ALIGN_MIDDLE); \
    } \
 \
    push_quad(group, pos, size, color, debug->window->layer + LAYER_MID); \
 \
    if (window_is_focused(debug, debug->window)) \
    { \
        if (point_inside_rect(debug->mouse_pos, pos, size)) \
        { \
            debug->next_hot_item.id = pointer; \
            debug->next_hot_item.ui_item_type = UI_ITEM_EDITBOX; \
        } \
    } \
} \

internal b8 
window_is_focused(DebugState* debug, DebugWindow* window)
{
    return debug->focused_window == window;
}

// TODO: handle collisions
internal DebugWindow*
get_window(DebugState* debug, char* key)
{
    u64 hash = 5381;
    i32 c;
    
    while((c = *key++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    u16 hash_index = hash % ArrayCount(debug->window_table);

    DebugWindow* debug_window = debug->window_table + hash_index;

    return debug_window;
}

internal b8
is_hot(DebugState* debug, void* id)
{
    return debug->hot_item.id == id;
}

internal b8
is_first_interaction(DebugState* debug, void* id)
{
    return (debug->interacting_item.id == id) &&
           (debug->is_first_interaction);
}

internal b8
is_interacting(DebugState* debug, void* id)
{
    return debug->interacting_item.id == id;
}

internal inline void
start_interaction(DebugState* debug, InterationType type)
{
    debug->interacting_item.id = debug->hot_item.id;
    debug->interacting_item.type = type;
    debug->is_first_interaction = true;
}

internal inline void
end_interaction(DebugState* debug)
{
    debug->interacting_item = {};
}

internal void 
process_debug_ui_interactions(DebugState* debug, GameInput* input)
{
    debug->is_first_interaction = false;

    if (debug->interacting_item.id)
    {
        switch(debug->interacting_item.type)
        {
            case INTERACTION_TYPE_CLICK:
            {
                end_interaction(debug);
            } break;
            case INTERACTION_TYPE_DRAG:
            {
                if (button_released(input->left_mouse_button))
                {
                    end_interaction(debug);
                }
            } break;
            case INTERACTION_TYPE_EDIT:
            {
                if (input->character != '\0')
                {
                    char c = input->character;
                    if ((c >= 48 && c < 58) ||
                        c == 46 ||
                        c == 43 ||
                        c == 45)
                    {
                        debug->text_input_buffer[debug->text_insert_index] = input->character;
                        debug->text_input_buffer[debug->text_insert_index + 1] = '\0';
                        ++debug->text_insert_index;
                    }
                }
                if (button_pressed(input->backspace) &&
                    debug->text_insert_index)
                {
                        --debug->text_insert_index;
                        debug->text_input_buffer[debug->text_insert_index] = '\0';
                }
                if (button_pressed(input->enter))
                {
                    if (debug->editbox_value_is_int)
                    {
                        i8 sign = 1;
                        // Delete minus sign
                        if (debug->text_input_buffer[0] == '-')
                        {
                            for (u32 i = 0; i < debug->text_insert_index; ++i)
                            {
                                debug->text_input_buffer[i] = debug->text_input_buffer[i+1];
                            }
                            sign = -1;
                        }

                        i64 number = (i64)atoi(debug->text_input_buffer);

                        if (debug->editbox_value_to_set_size == 1)
                        {
                            i8* int_to_set = (i8*)debug->editbox_value_to_set;
                            *int_to_set = ((i8)number) * sign;
                        }
                        else if (debug->editbox_value_to_set_size == 2)
                        {
                            i16* int_to_set = (i16*)debug->editbox_value_to_set;
                            *int_to_set = ((i16)number) * sign;
                        }
                        else if (debug->editbox_value_to_set_size == 4)
                        {
                            i32* int_to_set = (i32*)debug->editbox_value_to_set;
                            *int_to_set = ((i32)number) * sign;
                        }
                        else if (debug->editbox_value_to_set_size == 8)
                        {
                            i64* int_to_set = (i64*)debug->editbox_value_to_set;
                            *int_to_set = ((i64)number) * sign;
                        }
                    }
                    else
                    {
                        f64 number = (f64)atof(debug->text_input_buffer);
                        if (debug->editbox_value_to_set_size == 4)
                        {
                            f32* float_to_set = (f32*)debug->editbox_value_to_set;
                            *float_to_set = (f32)number;
                        }
                        else if (debug->editbox_value_to_set_size == 8)
                        {
                            f64* float_to_set = (f64*)debug->editbox_value_to_set;
                            *float_to_set = number;
                        }
                        else
                        {
                            ASSERT(false);
                        }
                    }
                    debug->editbox_value_to_set = 0;
                    debug->editbox_value_to_set_size = 0;
                    debug->editbox_value_is_int = 0;

                    end_interaction(debug);
                }
            } break;
        }
    }
    else if (debug->next_hot_item.id)
    {
        debug->hot_item = debug->next_hot_item;

        if (button_pressed(input->left_mouse_button))
        {
            if (debug->hot_item.ui_item_type == UI_ITEM_EDITBOX)
            {
                debug->text_insert_index = 0; 
                start_interaction(debug, INTERACTION_TYPE_EDIT);
            }
            else
            {
                start_interaction(debug, INTERACTION_TYPE_CLICK);
            }
        }
        else if (button_down(input->left_mouse_button))
        {
            start_interaction(debug, INTERACTION_TYPE_DRAG);
        }
        else if (button_pressed(input->right_mouse_button))
        {
            start_interaction(debug, INTERACTION_TYPE_EDIT);
        }
        debug->hot_item = debug->next_hot_item;
        debug->next_hot_item = {};
    }
    else
    {
        debug->hot_item = {};
    }
    debug->prev_mouse_pos = debug->mouse_pos;
    debug->mouse_pos = input->mouse.position;
}

enum TextAlign
{
    TEXT_ALIGN_MIDDLE,
    TEXT_ALIGN_LEFT,
};


internal inline void
debug_cursor_sameline(DebugState* debug)
{
    debug->is_newline = false;
}

internal inline void
debug_cursor_newline(DebugState* debug, f32 w, f32 h)
{
    if (debug->is_newline)
    {
        debug->draw_cursor.x = debug->window->position.x + PADDING;
        debug->draw_cursor.y -= h + PADDING;
    }
    else
    {
        debug->draw_cursor.x += w + PADDING;
    }
    debug->is_newline = true;
}

internal b8
name_is_too_long(Font* font, char* name)
{
    f32 xadvance = 0.0f;
    while(*name)
    {
        u32 index = *name-32;
        xadvance += font->char_metrics[index].xadvance;
        name++;
    }

    return (xadvance > MAX_NAME_WIDTH);
}

internal void
truncate_var_name(char* name)
{
    u32 len = string_length(name);
    name[len-1] = '\0';
    name[len-2] = '.';
    name[len-3] = '.';
}

internal char*
get_text_to_draw(MemoryArena* arena, u64 value)
{
    char* result = PushMemory(arena, char, 128);
    snprintf(result, 128, "%lu", value);

    return result;
}

internal char*
get_text_to_draw(MemoryArena* arena, f64 value)
{
    char* result = PushMemory(arena, char, 128);
    snprintf(result, 128, "%.2f", value);

    return result;
}

internal char*
get_text_to_draw(MemoryArena* arena, i64 value)
{
    char* result = PushMemory(arena, char, 128);
    snprintf(result, 128, "%li", value);

    return result;
}

internal char*
get_text_to_draw(MemoryArena* arena, u32 value)
{
    char* result = PushMemory(arena, char, 128);
    snprintf(result, 128, "%d", value);

    return result;
}

internal char*
get_text_to_draw(MemoryArena* arena, i32 value)
{
    char* result = PushMemory(arena, char, 128);
    snprintf(result, 128, "%d", value);

    return result;
}

internal char*
get_text_to_draw(MemoryArena* arena, f32 value)
{
    char* result = PushMemory(arena, char, 128);
    snprintf(result, 128, "%.2f", value);

    return result;
}

internal void
fill_text(DebugState* debug)
{
    debug->text_insert_index = string_length(debug->text_input_buffer);

    for (u32 i = debug->text_insert_index-1;;--i)
    {
        if (debug->text_input_buffer[i] == '0')
        {
            --debug->text_insert_index;
        }
        else
        {
            break;
        }
    }

    debug->text_input_buffer[debug->text_insert_index] = '\0';
}
internal void
fill_text_buffer(DebugState* debug, f64 value)
{
    snprintf(debug->text_input_buffer, 
             ArrayCount(debug->text_input_buffer), 
             "%f",
             value);
    fill_text(debug);
}

internal void
fill_text_buffer(DebugState* debug, f32 value)
{
    snprintf(debug->text_input_buffer, 
             ArrayCount(debug->text_input_buffer), 
             "%f",
             value);
    fill_text(debug);
}

internal void
fill_text_buffer(DebugState* debug, i64 value)
{
    snprintf(debug->text_input_buffer, 
             ArrayCount(debug->text_input_buffer), 
             "%li",
             value);
    debug->text_insert_index = string_length(debug->text_input_buffer);
    debug->text_input_buffer[debug->text_insert_index] = '\0';
}

internal void
fill_text_buffer(DebugState* debug, u64 value)
{
    snprintf(debug->text_input_buffer, 
             ArrayCount(debug->text_input_buffer), 
             "%lu",
             value);
    debug->text_insert_index = string_length(debug->text_input_buffer);
    debug->text_input_buffer[debug->text_insert_index] = '\0';
}

internal void
fill_text_buffer(DebugState* debug, u32 value)
{
    snprintf(debug->text_input_buffer, 
             ArrayCount(debug->text_input_buffer), 
             "%u",
             value);
    debug->text_insert_index = string_length(debug->text_input_buffer);
    debug->text_input_buffer[debug->text_insert_index] = '\0';
}

internal void
fill_text_buffer(DebugState* debug, i32 value)
{
    snprintf(debug->text_input_buffer, 
             ArrayCount(debug->text_input_buffer), 
             "%i",
             value);
    debug->text_insert_index = string_length(debug->text_input_buffer);
    debug->text_input_buffer[debug->text_insert_index] = '\0';
}

internal Font
debug_load_font(MemoryArena* arena, Platform* platform, Assets* assets, char* font_path, i32 font_size)
{
    Font font = {};
    font.char_subsprites = PushMemory(arena, SubSprite, NUM_ASCII);
    font.char_metrics = PushMemory(arena, CharMetric, NUM_ASCII);
    font.num_chars = NUM_ASCII;


    font.font_sprite_handle = create_empthy_sprite(assets, 512, 512, 1);
    font.font_size = font_size;

    Sprite* font_sprite = get_loaded_sprite(assets, font.font_sprite_handle);
    FileResult font_file = platform->read_entire_file(font_path);

    if (font_file.data)
    {

        stbtt_bakedchar char_metrics[NUM_ASCII];
        i32 result = stbtt_BakeFontBitmap((u8*)font_file.data, 
                                          0, (f32)font_size,
                                          (u8*)font_sprite->data, 
                                          font_sprite->width, 
                                          font_sprite->height,
                                          32, NUM_ASCII, 
                                          char_metrics);
        if (!result)
        {
            PRINT("Failed to bake font map\n");
        }

        for(u8 i = 0; i < NUM_ASCII; ++i)
        {
            #define IFLOOR(x) ((int) floor(x))
            stbtt_aligned_quad q;
            // i32 index = *text-32;
            stbtt_bakedchar* b = char_metrics + i;

            font.char_metrics[i].x0 = b->x0;
            font.char_metrics[i].y0 = b->y0;
            font.char_metrics[i].x1 = b->x1;
            font.char_metrics[i].y1 = b->y1;
            font.char_metrics[i].xoff = b->xoff;
            font.char_metrics[i].yoff = b->yoff;
            font.char_metrics[i].xadvance = b->xadvance;

            f32 ipw = 1.0f / font_sprite->width;
            f32 iph = 1.0f / font_sprite->height;

            q.s0 = b->x0 * ipw;
            q.t0 = b->y0 * iph;
            q.s1 = b->x1 * ipw;
            q.t1 = b->y1 * iph;

            SubSprite* subsprite = font.char_subsprites + i;

            subsprite->uvs[0] = {q.s1, q.t1};
            subsprite->uvs[1] = {q.s1, q.t0};
            subsprite->uvs[2] = {q.s0, q.t0};
            subsprite->uvs[3] = {q.s0, q.t1};

            subsprite->sprite_sheet = font.font_sprite_handle;
        }
        platform->free_file_memory(font_file.data);
    }
    else
    {
        PRINT("Failed to load font [%s] file.", font_path);
    }


    return font;
}

internal f32
get_text_width(Font* font, char* text)
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
debug_text(RenderGroup* render_group,
           Font* font,
           char* text,
           vec2 position,
           u32 layer,
           TextAlign align = TEXT_ALIGN_LEFT,
           Color color = {255, 255, 255, 255})
{
    if (align == TEXT_ALIGN_MIDDLE)
    {
        f32 text_width = get_text_width(font, text);
        position.x -= (text_width / 2.0f);
    }

    while(*text)
    {
        if (*text >= 32 && *text <= 128)
        {
            #define IFLOOR(x) ((int) floor(x))
            i32 index = *text-32;
            CharMetric* cm = font->char_metrics + index;

            i32 round_x = IFLOOR((position.x + cm->xoff) + 0.5f);
            i32 round_y = IFLOOR((position.y - cm->yoff) + 0.5f);

            vec2 pos = V2(round_x, round_y);
            vec2 size = V2(cm->x1 - cm->x0,
                           -cm->y1 + cm->y0);

            push_quad(render_group, font->char_subsprites + index, pos, size, color, layer);

            position.x += cm->xadvance;
        }
        text++;
    }
}

internal void
debug_submenu_titlebar(DebugState* debug, vec2 position, vec2 size, char* title)
{
    Color color = bg_main_color;

    if (is_interacting(debug, title))
    {
        switch(debug->interacting_item.type)
        {
            case INTERACTION_TYPE_CLICK:
            {
                color = faint_redblue_color;
                b8* active = &debug->window_items[debug->current_item_index].menu_is_active;
                *active = !(*active);
            } break;
        }
    }
    else if (is_hot(debug, title))
    {
        color = faint_red_color;
    }

    if (window_is_focused(debug, debug->window))
    {
        if (point_inside_rect(debug->mouse_pos,
                            position,
                            size))
        {
            debug->next_hot_item.id = title;
        }
    }

    push_quad(debug->render_group, position, size, color, debug->window->layer + LAYER_MID);
    debug_text(debug->render_group, 
               &debug->font, 
               title, 
               V2(position.x + (size.x / 2),
                  position.y + 2),
               debug->window->layer + LAYER_FRONT,
               TEXT_ALIGN_MIDDLE);
}

internal void
debug_ui(DebugState* debug, Assets* assets, Renderer* ren)
{

    if (debug->temp_arena.arena)
    {
        end_temporary_memory(&debug->temp_arena);
    }
    debug->temp_arena = begin_temporary_memory(&debug->arena);

    mat4 projection = mat4_orthographic((f32)ren->screen_width,
                                        (f32)ren->screen_height);
    Camera cam = {V3(0.0f, 0.0f, 1.0f), V3(0.0f, 0.0f, 1.0f), V3(0.0f, 1.0f, 0.0f)};
    debug->render_group = setup_render_group(&debug->arena,
                                             projection,
                                             cam,
                                             ren, 
                                             assets);
    debug->window = 0;
}

internal void
debug_window_begin(DebugState* debug, char* title)
{
    DebugWindow* window = get_window(debug, title);

    ASSERT(window);

    debug->window = window;


    if (window->position.x <= 0 || window->position.y <= 0)
    {
        window->size = V2(400);
        u32 x = (rand() % 1000) + 20;
        window->position = V2(x, 500);
    }


    Color color = faint_red_color;

    if (is_interacting(debug, title))
    {
        switch(debug->interacting_item.type)
        {
            case INTERACTION_TYPE_DRAG:
            {
                color = faint_red_color;
                vec2 mouse_delta = (debug->mouse_pos - debug->prev_mouse_pos);
                window->position = window->position + mouse_delta;
            } break;
            case INTERACTION_TYPE_CLICK:
            {
                debug->focused_window = window;
                debug->current_top_layer += 10;
                window->layer = debug->current_top_layer;
            } break;
        }
    }
    else if (is_hot(debug, title))
    {
        color = red_color;
    }
    debug->draw_cursor = window->position;
    debug->current_item_index = 0;


    vec2 titlebar_size = V2((f32)window->size.x, (f32)debug->font.font_size);

    if (!debug->focused_window)
    {
        debug->focused_window = debug->window;
    }

    if (point_inside_rect(debug->mouse_pos,
                          window->position,
                          titlebar_size))
    {
        if (window_is_focused(debug, debug->window))
        {
            debug->next_hot_item.id = title;
        }
        else
        {
            vec2 focused_window_pos = V2(debug->focused_window->position.x,
                                         debug->focused_window->position.y - debug->focused_window->size.y);
            vec2 window_pos = V2(window->position.x,
                                 window->position.y - window->size.y);
            vec2 window_size = V2(window->size.x,
                                  window->size.y + titlebar_size.y);

            if (point_inside_rect(debug->mouse_pos, window_pos, window_size) &&
               !point_inside_rect(debug->mouse_pos, focused_window_pos, debug->focused_window->size))
            {
                debug->next_hot_item.id = title;
            }
        }
    }

    push_quad(debug->render_group, window->position, titlebar_size, color, debug->window->layer + LAYER_MID);
    debug_text(debug->render_group, 
               &debug->font,
               title, 
               V2(window->position.x + (titlebar_size.x / 2),
                  window->position.y + 2),
               debug->window->layer + LAYER_FRONT,
               TEXT_ALIGN_MIDDLE);
}

internal void
debug_window_end(DebugState* debug)
{
    debug->window->size.y = debug->window->position.y - debug->draw_cursor.y;
    vec2 pos = V2(debug->window->position.x,
                  debug->window->position.y - debug->window->size.y);
    push_quad(debug->render_group, 
              pos,
              debug->window->size,
              faint_bg_color,
              debug->window->layer + LAYER_BACKMID);
}


internal void
debug_fps(DebugState* debug)
{
    RenderGroup* group = debug->render_group;
    char* fps_text = "%.2f fps";
    char out[32];
    sprintf(out, fps_text, debug->game_fps);
    vec2 position = V2(0.0f, (f32)group->renderer->screen_height - debug->font.font_size);

    debug_text(debug->render_group, &debug->font, out, position, 100);
}



internal void
debug_checkbox(DebugState* debug, b8* toggle_var, char* var_name)
{
    RenderGroup* group = debug->render_group;

    vec2 size = V2(20);
    debug_cursor_newline(debug, size.x, size.y);
    vec2 pos = debug->draw_cursor;

    if (name_is_too_long(&debug->font, var_name))
    {
        var_name = string_copy(&debug->arena, var_name);
        truncate_var_name(var_name);
    }
    debug_text(debug->render_group, 
               &debug->font, 
               var_name, 
               V2(pos.x, pos.y + (size.y - debug->font.font_size)),
               debug->window->layer + LAYER_FRONT);
    pos.x += MAX_NAME_WIDTH;

    push_quad(group, pos, size, bg_main_color, debug->window->layer + LAYER_BACKMID);

    Color check_color = {100, 100, 100, 255};

    if (is_interacting(debug, toggle_var))
    {
        if (debug->interacting_item.type == INTERACTION_TYPE_CLICK)
        {
            *toggle_var = !(*toggle_var);
        }
    }
    else if (is_hot(debug, toggle_var))
    {
        check_color = faint_red_color;
    }

    if (*toggle_var)
    {
        check_color.r *= 2;
        check_color.g *= 2;
        check_color.b *= 2;
    }

    push_quad(group, pos, size, bg_main_color, debug->window->layer + LAYER_BACKMID);
    size -= 8;
    pos += 4;
    push_quad(group, pos, size, check_color, debug->window->layer + LAYER_BACKMID);

    if (window_is_focused(debug, debug->window))
    {
        if (point_inside_rect(debug->mouse_pos,
                            pos,
                            size))
        {
            debug->next_hot_item.id = {toggle_var};
        }
    }
}

internal b8
debug_button(DebugState* debug, 
             char* name)
{
    RenderGroup* group = debug->render_group;
    vec2 button_size = V2(70, debug->font.font_size + 2.0f);

    debug_cursor_newline(debug, button_size.x, button_size.y);

    vec2 button_pos = debug->draw_cursor;
    Color button_color = bg_main_color;
    b8 result = false;


    if (is_interacting(debug, name))
    {
        result = true;
        button_color = red_color;
    }
    else if (is_hot(debug, name))
    {
        button_color = faint_red_color;
    }


    push_quad(group, button_pos, button_size, button_color, debug->window->layer + LAYER_MID);
    vec2 text_pos = V2(
        button_pos.x + (button_size.x / 2.0f),
        button_pos.y + (button_size.y / 4.0f)
    );
    debug_text(group, &debug->font, name, text_pos, debug->window->layer + LAYER_FRONT, TEXT_ALIGN_MIDDLE);

    if (window_is_focused(debug, debug->window))
    {
        if (point_inside_rect(debug->mouse_pos,
                            button_pos,
                            button_size))
        {
            debug->next_hot_item.id = {name};
        }
    }


    return result;
}




internal void
debug_slider(DebugState* debug, 
             f32 min,
             f32 max, 
             f32* value, 
             char* var_name)
{
    RenderGroup* group = debug->render_group;

    vec2 bar_size = V2(debug->window->size.x - MAX_NAME_WIDTH - 2*PADDING, 20.0f);
    debug_cursor_newline(debug, bar_size.x + MAX_NAME_WIDTH, bar_size.y);

    vec2 pos = debug->draw_cursor;


    if (name_is_too_long(&debug->font, var_name))
    {
        var_name = string_copy(&debug->arena, var_name);
        truncate_var_name(var_name);
    }
    debug_text(debug->render_group, 
               &debug->font, 
               var_name, 
               V2(pos.x, pos.y + (bar_size.y - debug->font.font_size)), 
               debug->window->layer + LAYER_FRONT);

    pos.x += MAX_NAME_WIDTH;

    vec2 button_position = pos;
    vec2 button_size = V2(15, bar_size.y);
    button_position.y -= (button_size.y - bar_size.y) / 2.0f;

    f32 range = max - min;


    Color button_color = faint_redblue_color;

    if (is_interacting(debug, value))
    {
        if (debug->interacting_item.type ==
            INTERACTION_TYPE_DRAG)
        {
            button_color = red_color;
            f32 slide_amount = (debug->mouse_pos.x - pos.x);
            slide_amount = CLAMP(slide_amount, 0.0f, bar_size.x - button_size.x);

            f32 update_value = slide_amount / (bar_size.x - button_size.x);

            *value = (range * update_value) + min;
        }
    }
    else if (is_hot(debug, value))
    {
        button_color = faint_red_color;
    }

    push_quad(group, pos, bar_size, bg_main_color, debug->window->layer + LAYER_BACKMID);


    vec2 text_pos = V2(pos.x+(bar_size.x/2), pos.y + (bar_size.y - debug->font.font_size));
    char text[32];
    snprintf(text, 32, "%.2f", *value);
    debug_text(debug->render_group, &debug->font, text, text_pos, debug->window->layer + LAYER_FRONT, TEXT_ALIGN_MIDDLE);

    button_position.x += (((*value)-min)/range) * (bar_size.x - button_size.x);
    push_quad(group, button_position, button_size, button_color, debug->window->layer + LAYER_MID);

    if (window_is_focused(debug, debug->window))
    {
        if (point_inside_rect(debug->mouse_pos, button_position, button_size))
        {
            debug->next_hot_item.id = {value};
        }
    }

}


internal void
debug_vec3_slider(DebugState* debug, vec3* v, char* name)
{
    debug_text(debug->render_group, &debug->font,  name, debug->draw_cursor, debug->window->layer + LAYER_FRONT);
    debug_slider(debug, 0.0f, 1.0f, &v->x, "x");
    debug_slider(debug, 0.0f, 1.0f, &v->y, "y");
    debug_slider(debug, 0.0f, 1.0f, &v->z, "z");
}

internal void
debug_color_slider(DebugState* debug, vec4* color)
{
    debug_slider(debug, 0.0f, 1.0f, &color->x, "r");
    debug_cursor_sameline(debug);
    debug_slider(debug, 0.0f, 1.0f, &color->y, "g");
    debug_cursor_sameline(debug);
    debug_slider(debug, 0.0f, 1.0f, &color->z, "b");
    debug_cursor_sameline(debug);
    debug_slider(debug, 0.0f, 1.0f, &color->w, "a");
    debug_cursor_sameline(debug);
}



internal b8
debug_submenu(DebugState* debug, char* title)
{
    debug->draw_cursor.y -=  debug->font.font_size + PADDING;
    debug->draw_cursor.x = debug->window->position.x + PADDING;


    debug_submenu_titlebar(debug, 
                           debug->draw_cursor, 
                           V2(debug->window->size.x - 2*PADDING,
                              (f32)debug->font.font_size),
                           title);

    b8 active = debug->window_items[debug->current_item_index].menu_is_active;

    ++debug->current_item_index;

    return active;
}


