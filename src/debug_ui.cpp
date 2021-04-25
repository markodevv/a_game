global_variable vec4 ui_color = {0.1f, 0.1f, 0.1f, 1.0f};
global_variable vec4 hover_color = {0.4f, 0.4f, 0.4f, 1.0f};
global_variable vec4 clicked_color = {1.0f, 1.0f, 1.0f, 1.0f};



internal inline b8
equals(UiItemId id_1, UiItemId id_2)
{
    return id_1.id == id_2.id;
}

internal b8
is_valid(InteractableUiItem* ui_item)
{
    return (ui_item->type != 0);
}

internal void 
process_debug_ui_interactions(DebugState* debug, GameInput* input)
{

    if (is_valid(&debug->interacting_item))
    {
        switch(debug->interacting_item.type)
        {
            case INTERACTABLE_TYPE_CLICK:
            {
                debug->interacting_item = {};
            } break;
            case INTERACTABLE_TYPE_DRAG:
            {
                if (button_released(input->left_mouse_button))
                {
                    debug->interacting_item = {};
                }
            } break;
        }
    }
    else if (is_valid(&debug->next_hot_item))
    {
        switch(debug->next_hot_item.type)
        {
            case INTERACTABLE_TYPE_CLICK:
            {
                if (button_released(input->left_mouse_button))
                {
                    debug->interacting_item = debug->next_hot_item;
                }
            } break;
            case INTERACTABLE_TYPE_DRAG:
            {
                if (button_down(input->left_mouse_button))
                {
                    debug->interacting_item = debug->next_hot_item;
                }
            } break;
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


    Image* font_image = get_loaded_image(group->assets, debug->font_image_handle);

    while(*text)
    {
        if (*text >= 32 && *text <= 128)
        {
            #define IFLOOR(x) ((int) floor(x))
            stbtt_aligned_quad q;
            i32 index = *text-32;
            stbtt_bakedchar* b = debug->char_metrics + index;

            float d3d_bias = 0.0f;
            f32 ipw = 1.0f / font_image->width;
            f32 iph = 1.0f / font_image->height;
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

            push_quad(group, debug->font_image_handle, positions, uvs, color);
            //push_quad(group, position, V2(40, 20), color);
        }
        text++;
    }
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



internal b8
debug_button(DebugState* debug, 
             char* name)
{
    RenderGroup* group = &debug->render_group;
    vec2 button_size = {70, debug->font_size + 10.0f};
    vec2 button_pos = debug->draw_cursor;
    vec4 button_color = ui_color;
    b8 result = false;


    if (equals(debug->interacting_item.id, {(void*)name}))
    {
        result = true;
        button_color = clicked_color;
    }
    else if (equals(debug->hot_item.id, {(void*)name}))
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
        debug->next_hot_item.type = INTERACTABLE_TYPE_CLICK;
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
    vec2 size = {100.0f, 10.0f};

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
    vec2 button_size = {15, 15};
    button_position.y -= (button_size.y - size.y) / 2.0f;

    f32 range = max - min;


    vec4 color = {0.5f, 0.5f, 0.5f, 1.0f};

    if (equals(debug->interacting_item.id, {value}))
    {
        if (debug->interacting_item.type ==
            INTERACTABLE_TYPE_DRAG)
        {
            color = clicked_color;
        }
        f32 slide_amount = (debug->mouse_pos.x - pos.x);
        slide_amount = CLAMP(slide_amount, 0.0f, size.x);

        f32 update_value = slide_amount / size.x;

        *value = (range * update_value) + min;
    }
    else if (equals(debug->hot_item.id, {value}))
    {
        color = hover_color;
    }

    push_quad(group, pos, {size.x+button_size.x, size.y}, ui_color);
    vec2 text_pos = {(pos.x+size.x+button_size.x), pos.y};


    char *text = PushMemory(&debug->arena, char, 30);
    sprintf_s(text, 30, "%.2f", *value);
    debug_text(debug, text, text_pos);

    button_position.x += (((*value)-min)/range) * size.x;
    push_quad(group, button_position, button_size, color);

    if (point_is_inside(debug->mouse_pos, button_position, button_size))
    {
        debug->next_hot_item.id = {value};
        debug->next_hot_item.type = INTERACTABLE_TYPE_DRAG;
    }
    else if (point_is_inside(debug->mouse_pos, pos, size))
    {
        debug->next_hot_item.id = {value};
        debug->next_hot_item.type = INTERACTABLE_TYPE_CLICK;
    }

    debug->draw_cursor.x += text_box_size(debug, text).x + 
                            size.x + 
                            max_name_x +
                            X_PADDING;

    end_temporary_memory(&temp_arena);
}

internal void
debug_menu_newline(DebugState* debug)
{
    debug->prev_draw_cursor.y -= debug->font_size + Y_PADDING;
    debug->draw_cursor = debug->prev_draw_cursor;
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


internal void
debug_menu_begin(DebugState* debug, 
                 vec2 pos, 
                 vec2 size,
                 char* menu_name)
{
    RenderGroup* group = &debug->render_group;
    vec4 color = ui_color;
    pos = debug->menu_pos;

    if (equals(debug->interacting_item.id, {menu_name}))
    {
        debug->menu_pos = debug->menu_pos + (debug->mouse_pos - debug->prev_mouse_pos);
        color = clicked_color;
    }
    else if (equals(debug->hot_item.id, {menu_name}))
    {
        color = hover_color;
    }

    debug->prev_draw_cursor = pos;
    vec2 header_size = {size.x, debug->font_size + 5.0f};
    vec2 header_pos = {pos.x, pos.y + size.y - header_size.y};
    push_quad(group, pos, size, {0.5f, 0.5f, 0.5f, 0.5f});
    push_quad(group, header_pos, header_size, color);

    pos = header_pos;
    pos.x += size.x / 2.0f;
    pos.y += 5.0f;
    debug_text(debug, menu_name, pos, TEXT_ALIGN_MIDDLE);

    debug->prev_draw_cursor.y += size.y - 2.0f * header_size.y;
    debug->prev_draw_cursor.x += X_PADDING;

    if (point_is_inside(debug->mouse_pos,
                        header_pos,
                        header_size))
    {
        debug->next_hot_item.id = {menu_name};
        debug->next_hot_item.type = INTERACTABLE_TYPE_DRAG;
    }

    debug->draw_cursor = debug->prev_draw_cursor;
}

