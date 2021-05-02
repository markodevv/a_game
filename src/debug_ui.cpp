
global_variable vec4 ui_color = {0.1f, 0.1f, 0.1f, 1.0f};
global_variable vec4 hover_color = {0.4f, 0.4f, 0.4f, 1.0f};
global_variable vec4 clicked_color = {1.0f, 1.0f, 1.0f, 1.0f};


internal b8
is_hot(DebugState* debug, void* id)
{
    return debug->hot_item.id == id;
}

internal b8
is_interacting(DebugState* debug, void* id)
{
    return debug->interacting_item.id == id;
}

internal void 
process_debug_ui_interactions(DebugState* debug, GameInput* input)
{

    if (debug->interacting_item.id)
    {
        switch(debug->interacting_item.type)
        {
            case INTERACTION_TYPE_CLICK:
            {
                debug->interacting_item = {};
            } break;
            case INTERACTION_TYPE_DRAG:
            {
                if (button_released(input->left_mouse_button))
                {
                    debug->interacting_item = {};
                }
            } break;
        }
    }
    else if (debug->next_hot_item.id)
    {
        debug->hot_item = debug->next_hot_item;

        if (button_pressed(input->left_mouse_button))
        {
            debug->interacting_item = debug->hot_item;
            debug->interacting_item.type = INTERACTION_TYPE_CLICK;
        }
        else if (button_down(input->left_mouse_button))
        {
            debug->interacting_item = debug->hot_item;
            debug->interacting_item.type = INTERACTION_TYPE_DRAG;
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

internal void
debug_text(DebugState* debug,
           char* text,
           vec2 position,
           TextAlign align = TEXT_ALIGN_LEFT,
           vec4 color = {1.0f, 1.0f, 1.0f, 1.0f})
{
    RenderGroup* group = &debug->render_group;
    if (align == TEXT_ALIGN_MIDDLE)
    {
        vec2 text_size = text_box_size(debug, text);
        position.x -= text_size.x / 2.0f;
    }


    Sprite* font_sprite = get_loaded_sprite(group->assets, debug->font_sprite_handle);

    while(*text)
    {
        if (*text >= 32 && *text <= 128)
        {
            #define IFLOOR(x) ((int) floor(x))
            stbtt_aligned_quad q;
            i32 index = *text-32;
            stbtt_bakedchar* b = debug->char_metrics + index;

            float d3d_bias = 0.0f;
            f32 ipw = 1.0f / font_sprite->width;
            f32 iph = 1.0f / font_sprite->height;
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

            vec2 positions[] =
            {
                {q.x0, q.y0},
                {q.x0, q.y1},
                {q.x1, q.y1},
                {q.x0, q.y0},
                {q.x1, q.y1},
                {q.x1, q.y0},
            };

            vec2 uvs[] =
            {
                {q.s0, q.t1},
                {q.s0, q.t0},
                {q.s1, q.t0},
                {q.s0, q.t1},
                {q.s1, q.t0},
                {q.s1, q.t1},
            };

            push_quad(group, debug->font_sprite_handle, positions, uvs, color);
            //push_quad(group, position, V2(40, 20), color);
        }
        text++;
    }
}

internal void
debug_menu_titlebar(DebugState* debug, vec2 position, vec2 size, char* title, b8 submenu)
{
    vec4 color = ui_color;

    if (is_interacting(debug, title))
    {
        switch(debug->interacting_item.type)
        {
            case INTERACTION_TYPE_CLICK:
            {
                color = clicked_color;
            } break;
            if (!submenu)
            {
                case INTERACTION_TYPE_DRAG:
                {
                    color = hover_color;
                    debug->menu_pos = debug->menu_pos + (debug->mouse_pos - debug->prev_mouse_pos);
                } break;
            }
        }
    }
    else if (is_hot(debug, title))
    {
        color = hover_color;
    }

    if (point_is_inside(debug->mouse_pos,
                        position,
                        size))
    {
        debug->next_hot_item.id = title;
    }

    push_quad(&debug->render_group, position, size, color);
    debug_text(debug, 
               title, 
               V2(position.x + (size.x / 2),
                  position.y + 2),
               TEXT_ALIGN_MIDDLE);
}

internal void
debug_ui_begin(DebugState* debug, i32 screen_width, i32 screen_height, char* title)
{
    debug->menu_size = V2((f32)400, (f32)screen_height);
    vec2 titlebar_pos = debug->menu_pos + V2(0, screen_height - debug->font_size);
    vec2 titlebar_size = V2((f32)400, debug->font_size);

    debug->draw_cursor = V2(0);

    push_quad(&debug->render_group, debug->menu_pos, debug->menu_size, V4(0.5f));

    debug_menu_titlebar(debug, titlebar_pos, titlebar_size, title, false);

    debug->draw_cursor.x = debug->menu_pos.x + debug->font_size/4;
    debug->draw_cursor.y = debug->menu_pos.y + debug->menu_size.y - (debug->font_size * 2 + 5);

    debug->current_menu_index = 0;
}


internal void
debug_fps(DebugState* debug)
{
    RenderGroup* group = &debug->render_group;
    char* fps_text = "%.2f fps";
    char out[32];
    sprintf_s(out, fps_text, debug->game_fps);
    vec2 position = V2(0.0f, (f32)group->renderer->screen_height - debug->font_size);

    debug_text(debug, out, position);
}


#define X_PADDING 20.0f
#define Y_PADDING 10.0f

internal void
debug_checkbox(DebugState* debug, vec2 position, b8* toggle_var)
{
    RenderGroup* group = &debug->render_group;

    vec2 size = V2(debug->font_size);
    push_quad(group, position, size, ui_color);

    vec4 check_color = V4(0.5f, 0.5f, 0.5f, 1.0f);

    if (is_interacting(debug, toggle_var))
    {
        if (debug->interacting_item.type == INTERACTION_TYPE_CLICK)
        {
            *toggle_var = !(*toggle_var);
        }
    }
    else if (is_hot(debug, toggle_var))
    {
        check_color = hover_color;
    }

    if (*toggle_var)
    {
        check_color *= 3.0f;
        check_color.w = 1.0f;
    }

    size -= 4;
    position += 2;
    push_quad(group, position, size, check_color);

    if (point_is_inside(debug->mouse_pos,
                        position,
                        size))
    {
        debug->next_hot_item.id = {toggle_var};
    }
}

internal b8
debug_button(DebugState* debug, 
             char* name)
{
    RenderGroup* group = &debug->render_group;
    vec2 button_size = V2(70, debug->font_size + 2.0f);
    vec2 button_pos = debug->draw_cursor;
    vec4 button_color = ui_color;
    b8 result = false;


    if (is_interacting(debug, name))
    {
        result = true;
        button_color = clicked_color;
    }
    else if (is_hot(debug, name))
    {
        button_color = hover_color;
    }


    push_quad(group, button_pos, button_size, button_color);
    vec2 text_pos = V2(
        button_pos.x + (button_size.x / 2.0f),
        button_pos.y + (button_size.y / 4.0f)
    );
    debug_text(debug, name, text_pos, TEXT_ALIGN_MIDDLE);

    if (point_is_inside(debug->mouse_pos,
                        button_pos,
                        button_size))
    {
        debug->next_hot_item.id = {name};
        debug->next_hot_item.type = INTERACTION_TYPE_CLICK;
    }

    debug->draw_cursor.x += button_size.x + X_PADDING;
    return result;
}

internal void
debug_slider(DebugState* debug, 
             f32 min,
             f32 max, 
             f32* value, 
             char* var_name)
{
    TemporaryArena temp_arena = begin_temporary_memory(&debug->arena);
    RenderGroup* group = &debug->render_group;

    vec2 pos = debug->draw_cursor;
    vec2 size = V2(100.0f, 10.0f);

    char* name_copy = string_copy(&debug->arena, var_name);
    f32 name_x = debug->x_advance * string_length(name_copy);
    f32 max_name_x = debug->x_advance * MAX_UI_VAR_NAME_LEN;

    if (name_x > max_name_x)
    {
        name_copy[MAX_UI_VAR_NAME_LEN] = '\0';
        name_copy[MAX_UI_VAR_NAME_LEN-1] = '.';
        name_copy[MAX_UI_VAR_NAME_LEN-2] = '.';
    }

    debug_text(debug, name_copy, pos);
    pos.x += max_name_x;

    vec2 button_position = pos;
    vec2 button_size = V2(15, 15);
    button_position.x -= button_size.x / 2;
    button_position.y -= (button_size.y - size.y) / 2.0f;

    f32 range = max - min;


    vec4 color = V4(0.5f, 0.5f, 0.5f, 1.0f);

    if (is_interacting(debug, value))
    {
        if (debug->interacting_item.type ==
            INTERACTION_TYPE_DRAG)
        {
            color = clicked_color;
        }
        f32 slide_amount = (debug->mouse_pos.x - pos.x);
        slide_amount = CLAMP(slide_amount, 0.0f, size.x);

        f32 update_value = slide_amount / size.x;

        *value = (range * update_value) + min;
    }
    else if (is_hot(debug, value))
    {
        color = hover_color;
    }

    push_quad(group, pos, size, ui_color);
    vec2 text_pos = V2(pos.x+(size.x/2), pos.y);


    char *text = PushMemory(&debug->arena, char, 30);
    sprintf_s(text, 30, "%.2f", *value);
    debug_text(debug, text, text_pos, TEXT_ALIGN_MIDDLE);

    button_position.x += (((*value)-min)/range) * size.x;
    push_quad(group, button_position, button_size, color);

    if (point_is_inside(debug->mouse_pos, button_position, button_size))
    {
        debug->next_hot_item.id = {value};
        debug->next_hot_item.type = INTERACTION_TYPE_DRAG;
    }
    else if (point_is_inside(debug->mouse_pos, pos, size))
    {
        debug->next_hot_item.id = {value};
        debug->next_hot_item.type = INTERACTION_TYPE_CLICK;
    }

    debug->draw_cursor.x += text_box_size(debug, text).x + 
                            size.x + 
                            max_name_x +
                            X_PADDING;

    end_temporary_memory(&temp_arena);
}

internal inline void
debug_cursor_indent(DebugState* debug)
{
    debug->draw_cursor.y -= (debug->font_size + 5) + debug->font_size;
    debug->draw_cursor.x += debug->font_size/4;
}

internal inline void
debug_cursor_reindent(DebugState* debug)
{
    debug->draw_cursor.x -= debug->font_size/4;
}

internal void
debug_menu_newline(DebugState* debug)
{
    debug->draw_cursor.y -= debug->font_size;
    debug->draw_cursor.x = debug->cursor_start_x;
}

internal void
debug_vec3_slider(DebugState* debug, vec3* v, char* name)
{
    debug_menu_newline(debug);
    debug_text(debug, name, debug->draw_cursor);
    debug_menu_newline(debug);

    debug_slider(debug, 0.0f, 1.0f, &v->x, "x");
    debug_menu_newline(debug);
    debug_slider(debug, 0.0f, 1.0f, &v->y, "y");
    debug_menu_newline(debug);
    debug_slider(debug, 0.0f, 1.0f, &v->z, "z");
    debug_menu_newline(debug);
}



internal b8
debug_menu_begin(DebugState* debug, char* title)
{
    RenderGroup* group = &debug->render_group;
    vec4 color = ui_color;


// Title bar
    debug_menu_titlebar(debug, 
                        debug->draw_cursor, 
                        V2(debug->menu_size.x - debug->font_size/2,
                           debug->font_size),
                        title,
                        true);

    debug_checkbox(debug, debug->draw_cursor, &debug->menus[debug->current_menu_index].menu_is_active);
                           

    debug_cursor_indent(debug);

    debug->cursor_start_x = debug->draw_cursor.x;

    return debug->menus[debug->current_menu_index].menu_is_active;
}

internal void
debug_menu_end(DebugState* debug)
{

    ++debug->current_menu_index;
    debug_cursor_reindent(debug);
}

