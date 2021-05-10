global_variable Color ui_color = {50, 50, 50, 255};
global_variable Color hover_color = {120, 120, 120, 255};
global_variable Color clicked_color = {255, 255, 255, 255};

#define PADDING 4.0f
#define MAX_NAME_WIDTH 120

// NOTE: macros below only work with 4byte per component types
#define DebugFloatEditbox(debug, var, name) \
    debug_generic_editbox(debug, var, DEBUG_VAR_FLOAT, (sizeof(*var)/sizeof(f32)), name);

#define DebugIntEditbox(debug, var, name) \
    debug_generic_editbox(debug, var, DEBUG_VAR_INT, (sizeof(*var)/sizeof(i32)), name);

internal b8
is_hot(DebugState* debug, void* id)
{
    return debug->hot_item.id == id;
}

internal b8
is_first_interaction(DebugState* debug, void* id)
{
    return debug->first_interaction_check.id == id;
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
    debug->first_interaction_check = debug->interacting_item;
}

internal inline void
end_interaction(DebugState* debug)
{
    debug->interacting_item = {};
}

internal void 
process_debug_ui_interactions(DebugState* debug, GameInput* input)
{
    debug->first_interaction_check = {};

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
                if (input->character != -1)
                {
                    char c = input->character;
                    if (c >= 48 &&
                        c < 58)
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
                if (button_pressed(input->escape))
                {
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
        debug->draw_cursor.x = debug->menu_pos.x + PADDING;
        debug->draw_cursor.y -= h + PADDING;
    }
    else
    {
        debug->draw_cursor.x += w + PADDING;
    }
    debug->is_newline = true;
}

internal void
truncate_var_name(DebugState* debug, char* name)
{
    f32 name_render_width = debug->x_advance * string_length(name);

    if (name_render_width > MAX_NAME_WIDTH)
    {
        u32 max_name_len = MAX_NAME_WIDTH / (u32)debug->x_advance;
        name[max_name_len] = '\0';
        name[max_name_len-1] = '.';
        name[max_name_len-2] = '.';
    }

}


internal void
fill_text_buffer(DebugState* debug, f32 value)
{
    snprintf(debug->text_input_buffer, 
             ArrayCount(debug->text_input_buffer), 
             "%f",
             value);
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
fill_text_buffer(DebugState* debug, i32 value)
{
    snprintf(debug->text_input_buffer, 
             ArrayCount(debug->text_input_buffer), 
             "%d",
             value);
    debug->text_insert_index = string_length(debug->text_input_buffer);
    debug->text_input_buffer[debug->text_insert_index] = '\0';
}

internal void
debug_text(DebugState* debug,
           char* text,
           vec2 position,
           TextAlign align = TEXT_ALIGN_LEFT,
           Color color = {255, 255, 255, 255})
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
                {q.x1, q.y0},
                {q.x1, q.y1},
                {q.x0, q.y1},
                {q.x0, q.y0},
            };

            vec2 uvs[] =
            {
                {q.s1, q.t1},
                {q.s1, q.t0},
                {q.s0, q.t0},
                {q.s0, q.t1},
            };

            push_quad(group, debug->font_sprite_handle, positions, uvs, color);
        }
        text++;
    }
}

internal void
debug_submenu_titlebar(DebugState* debug, vec2 position, vec2 size, char* title)
{
    Color color = ui_color;

    if (is_interacting(debug, title))
    {
        switch(debug->interacting_item.type)
        {
            case INTERACTION_TYPE_CLICK:
            {
                color = clicked_color;
                b8* active = &debug->menus[debug->current_menu_index].menu_is_active;
                *active = !(*active);
            } break;
        }
    }
    else if (is_hot(debug, title))
    {
        color = hover_color;
    }

    if (point_rect_intersect(debug->mouse_pos,
                        position,
                        size))
    {
        debug->next_hot_item.id = title;
    }

    push_quad(&debug->render_group, position, size, color, UI_LAYER_BACKMID);
    debug_text(debug, 
               title, 
               V2(position.x + (size.x / 2),
                  position.y + 2),
               TEXT_ALIGN_MIDDLE);

}

internal void
debug_menu_titlebar(DebugState* debug, char* title)
{
    Color color = ui_color;

    if (is_interacting(debug, title))
    {
        switch(debug->interacting_item.type)
        {
            case INTERACTION_TYPE_DRAG:
            {
                color = hover_color;
                debug->menu_pos = debug->menu_pos + (debug->mouse_pos - debug->prev_mouse_pos);
            } break;
        }
    }
    else if (is_hot(debug, title))
    {
        color = hover_color;
    }

    vec2 position = debug->menu_pos + V2(0, debug->menu_size.y - debug->font_size);
    vec2 size = V2((f32)400, debug->font_size);

    if (point_rect_intersect(debug->mouse_pos,
                        position,
                        size))
    {
        debug->next_hot_item.id = title;
    }

    push_quad(&debug->render_group, position, size, color, UI_LAYER_BACKMID);
    debug_text(debug, 
               title, 
               V2(position.x + (size.x / 2),
                  position.y + 2),
               TEXT_ALIGN_MIDDLE);

}

internal void
debug_ui_begin(DebugState* debug, Assets* assets, Renderer* ren, i32 screen_width, i32 screen_height, char* title)
{
    debug->render_setup.projection = mat4_orthographic((f32)ren->screen_width,
                                                       (f32)ren->screen_height);
    debug->render_setup.camera = {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}};
    debug->render_group = render_group_begin(&debug->render_setup,
                                             ren, 
                                             assets);

    debug->menu_size = V2((f32)400, 500);

    debug->draw_cursor = V2(0);


    debug_menu_titlebar(debug, title);

    push_quad(&debug->render_group, debug->menu_pos, debug->menu_size, COLOR(122, 122, 122, 122), UI_LAYER_BACKMID);

    debug->draw_cursor.x = debug->menu_pos.x + PADDING;
    debug->draw_cursor.y = debug->menu_pos.y + debug->menu_size.y - (2 * debug->font_size) - PADDING;


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



internal void
debug_checkbox(DebugState* debug, b8* toggle_var, char* var_name)
{
    RenderGroup* group = &debug->render_group;

    vec2 size = V2(20);
    debug_cursor_newline(debug, size.x, size.y);
    vec2 pos = debug->draw_cursor;

    truncate_var_name(debug, var_name);
    debug_text(debug, var_name, V2(pos.x, pos.y + (size.y - debug->font_size)));
    pos.x += MAX_NAME_WIDTH;

    push_quad(group, pos, size, ui_color, UI_LAYER_BACKMID);

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
        check_color = hover_color;
    }

    if (*toggle_var)
    {
        check_color.r *= 2;
        check_color.g *= 2;
        check_color.b *= 2;
    }

    push_quad(group, pos, size, ui_color, UI_LAYER_BACKMID);
    size -= 8;
    pos += 4;
    push_quad(group, pos, size, check_color, UI_LAYER_BACKMID);

    if (point_rect_intersect(debug->mouse_pos,
                        pos,
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

    debug_cursor_newline(debug, button_size.x, button_size.y);

    vec2 button_pos = debug->draw_cursor;
    Color button_color = ui_color;
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


    push_quad(group, button_pos, button_size, button_color, UI_LAYER_BACKMID);
    vec2 text_pos = V2(
        button_pos.x + (button_size.x / 2.0f),
        button_pos.y + (button_size.y / 4.0f)
    );
    debug_text(debug, name, text_pos, TEXT_ALIGN_MIDDLE);

    if (point_rect_intersect(debug->mouse_pos,
                        button_pos,
                        button_size))
    {
        debug->next_hot_item.id = {name};
    }


    return result;
}

enum DebugVariableType
{
    DEBUG_VAR_FLOAT,
    DEBUG_VAR_INT,
};

internal void
draw_variabe(DebugState* debug, void* value, vec2 pos, vec2 size, DebugVariableType type)
{
    if (is_first_interaction(debug, value))
    {
        if (type == DEBUG_VAR_FLOAT)
            fill_text_buffer(debug,  *((f32*)value));
        else if (type == DEBUG_VAR_INT)
            fill_text_buffer(debug,  *((i32*)value));
    }

    char text[32];

    if (type == DEBUG_VAR_FLOAT)
    {
        snprintf(text, 32, "%.2f", *((f32*)(value)));
    }
    else if (type == DEBUG_VAR_INT)
    {
        snprintf(text, 32, "%d", *((i32*)(value)));
    }

    b8 is_editing_box = is_interacting(debug, value) && 
                        (debug->interacting_item.type == INTERACTION_TYPE_EDIT);

    if (is_editing_box)
    {
        debug_text(debug, debug->text_input_buffer, V2(pos.x, pos.y + (size.y - debug->font_size)));

        debug->text_input_buffer[debug->text_insert_index] = '\0';

        if (type == DEBUG_VAR_FLOAT)
            *((f32*)value) = (f32)atof(debug->text_input_buffer);
        else if (type == DEBUG_VAR_INT)
            *((i32*)value) = (i32)atoi(debug->text_input_buffer);

        f32 cursor_x = pos.x + debug->x_advance * string_length(debug->text_input_buffer);
        push_quad(&debug->render_group, V2(cursor_x, pos.y), V2(1, size.y), {255, 0, 0, 255}, UI_LAYER_BACKMID);
    }
    else
    {
        debug_text(debug, 
                   text, 
                   V2(pos.x + (size.x / 2), 
                      pos.y + (size.y - debug->font_size)),
                   TEXT_ALIGN_MIDDLE);
    }
}


internal void
debug_editbox(DebugState* debug, void* value, char* var_name, DebugVariableType type, vec2 size = V2(240.0f, 20.0f))
{
    RenderGroup* group = &debug->render_group;

    if (var_name)
    {
        debug_cursor_newline(debug, size.x + MAX_NAME_WIDTH, size.y);
    }
    else
    {
        debug_cursor_newline(debug, size.x, size.y);
    }
    vec2 pos = debug->draw_cursor;
    Color color = ui_color;

    if (is_interacting(debug, value))
    {
    }
    else if (is_hot(debug, value))
    {
        color.r *= 2;
        color.g *= 2;
        color.b *= 2;
    }


    if (var_name)
    {
        truncate_var_name(debug, var_name);
        debug_text(debug, var_name, V2(pos.x, pos.y + (size.y - debug->font_size)));
    }

    pos.x += MAX_NAME_WIDTH;
    push_quad(group, pos, size, color, UI_LAYER_BACKMID);

    draw_variabe(debug, value, pos, size, type);

    if (point_rect_intersect(debug->mouse_pos, pos, size))
    {
        debug->next_hot_item.id = value;
        debug->next_hot_item.ui_item_type = UI_ITEM_EDITBOX;
    }
}


internal void
debug_generic_editbox(DebugState* debug, void* v, DebugVariableType type, u32 num_components, char* var_name)
{
    f32 total_width = 240.0f - ((num_components-1) * PADDING);
    debug_editbox(debug, v, var_name, type, V2(total_width/num_components, 20));

    for (u32 i = 1; i < num_components; ++i)
    {
        debug_cursor_sameline(debug);
        debug_editbox(debug, (((u8*)v)+(i*4)), 0, type, V2(total_width/num_components, 20));
    }
}

internal void
debug_slider(DebugState* debug, 
             f32 min,
             f32 max, 
             f32* value, 
             char* var_name)
{
    RenderGroup* group = &debug->render_group;

    vec2 bar_size = V2(240.0f, 20.0f);
    debug_cursor_newline(debug, bar_size.x + MAX_NAME_WIDTH, bar_size.y);

    vec2 pos = debug->draw_cursor;


    truncate_var_name(debug, var_name);
    debug_text(debug, var_name, V2(pos.x, pos.y + (bar_size.y - debug->font_size)));

    pos.x += MAX_NAME_WIDTH;

    vec2 button_position = pos;
    vec2 button_size = V2(15, bar_size.y);
    button_position.y -= (button_size.y - bar_size.y) / 2.0f;

    f32 range = max - min;


    Color button_color = {255, 255, 255, 255};

    if (is_interacting(debug, value))
    {
        if (debug->interacting_item.type ==
            INTERACTION_TYPE_DRAG)
        {
            button_color = clicked_color;
            button_color.a /= 2;
            f32 slide_amount = (debug->mouse_pos.x - pos.x);
            slide_amount = CLAMP(slide_amount, 0.0f, bar_size.x - button_size.x);

            f32 update_value = slide_amount / (bar_size.x - button_size.x);

            *value = (range * update_value) + min;
        }
    }
    else if (is_hot(debug, value))
    {
        button_color = hover_color;
        button_color.a /= 2;
    }

    push_quad(group, pos, bar_size, ui_color, UI_LAYER_BACKMID);


    vec2 text_pos = V2(pos.x+(bar_size.x/2), pos.y + (bar_size.y - debug->font_size));
    char text[32];
    snprintf(text, 32, "%.2f", *value);
    debug_text(debug, text, text_pos, TEXT_ALIGN_MIDDLE);

    button_position.x += (((*value)-min)/range) * (bar_size.x - button_size.x);
    push_quad(group, button_position, button_size, button_color, UI_LAYER_BACKMID);

    if (point_rect_intersect(debug->mouse_pos, button_position, button_size))
    {
        debug->next_hot_item.id = {value};
    }

}


internal void
debug_vec3_slider(DebugState* debug, vec3* v, char* name)
{
    debug_text(debug, name, debug->draw_cursor);
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
debug_menu_begin(DebugState* debug, char* title)
{
    RenderGroup* group = &debug->render_group;
    Color color = ui_color;


    debug_submenu_titlebar(debug, 
                           debug->draw_cursor, 
                           V2(debug->menu_size.x - 2*PADDING,
                              debug->font_size),
                           title);

    b8 active = debug->menus[debug->current_menu_index].menu_is_active;

    return active;
}

internal void
debug_menu_end(DebugState* debug)
{
    debug->draw_cursor.y -=  debug->font_size + PADDING;
    debug->draw_cursor.x = debug->menu_pos.x + PADDING;

    ++debug->current_menu_index;
}

