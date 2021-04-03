global_variable vec4 ui_color = {0.1f, 0.1f, 0.1f, 1.0f};
global_variable vec4 hover_color = {0.4f, 0.4f, 0.4f, 1.0f};
global_variable vec4 clicked_color = {1.0f, 1.0f, 1.0f, 1.0f};


internal InteractableUiItem*
push_interactable_item(MemoryArena* arena, InteractableUiItemList* ui_item_list)
{
    InteractableUiItem* ui_item = PushMemory(arena, InteractableUiItem);
    if (!ui_item_list->head)
    {
        ui_item->next_item = NULL;
        ui_item_list->head = ui_item;
        ui_item_list->tail = ui_item;
    }
    else
    {
        ui_item->next_item = NULL;
        ui_item_list->tail->next_item = ui_item;
        ui_item_list->tail = ui_item;
    }

    return ui_item;
}

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
    else if (is_valid(&debug->hot_item))
    {
        switch(debug->hot_item.type)
        {
            case INTERACTABLE_TYPE_CLICK:
            {
                if (button_released(input->left_mouse_button))
                {
                    debug->interacting_item = debug->hot_item;
                }
            } break;
            case INTERACTABLE_TYPE_DRAG:
            {
                if (button_down(input->left_mouse_button))
                {
                    debug->interacting_item = debug->hot_item;
                }
            } break;
        }
        debug->next_hot_item = debug->hot_item;
        debug->hot_item = {};
    }
    else
    {
        debug->next_hot_item = {};
    }
    debug->prev_mouse_pos = debug->mouse_pos;
    debug->mouse_pos = input->mouse.position;
}

internal void
draw_debug_fps(Renderer* ren, DebugState* debug)
{
    char* fps_text = "%.2f fps";
    char out[32];
    sprintf_s(out, fps_text, debug->game_fps);
    vec3 position = {0.0f,
                     (f32)ren->screen_height - ren->font_size,
                     0.0f};

    draw_debug_text(ren, out, position);
}

internal void
debug_begin(DebugState* debug)
{
    debug->temporary_arena = begin_temporary_memory(&debug->arena);
}

internal void
debug_end(DebugState* debug)
{
    if (debug->temporary_arena.arena)
    {
        end_temporary_memory(&debug->temporary_arena);
        debug->ui_item_list.head = NULL;
        debug->ui_item_list.tail = debug->ui_item_list.head;
    }
}

#define UI_PADDING 10.0f

internal b8
draw_debug_button(Renderer* ren, 
                  DebugState* debug, 
                  char* name)
{
    vec2 button_size = {70, ren->font_size + UI_PADDING / 2.0f};
    vec2 button_pos = debug->draw_cursor;
    vec4 button_color = ui_color;
    b8 result = false;



    if (equals(debug->interacting_item.id, {(void*)name}))
    {
        result = true;
        button_color = clicked_color;
    }
    else if (equals(debug->next_hot_item.id, {(void*)name}))
    {
        button_color = hover_color;
    }


    draw_rect(ren, V3(button_pos), button_size, button_color);
    vec3 text_pos = {
        button_pos.x + (button_size.x / 2.0f),
        button_pos.y + (button_size.y / 4.0f),
        0.0f,
    };
    draw_debug_text(ren, name, text_pos, TEXT_ALIGN_MIDDLE);

    if (point_is_inside(debug->mouse_pos,
                        button_pos,
                        button_size))
    {
        debug->hot_item.id = {name};
        debug->hot_item.type = INTERACTABLE_TYPE_CLICK;
    }

    debug->draw_cursor.x += button_size.x + UI_PADDING;
    return result;
}

internal void
draw_debug_slider(Renderer* ren, 
                  DebugState* debug, 
// TODO: not working with negative values
                  f32 min,
                  f32 max, 
                  f32* value, 
                  char* var_name)
{
    vec2 pos = debug->draw_cursor;
    vec2 size = {100.0f, 10.0f};

    vec2 button_position = debug->draw_cursor;
    vec2 button_size = {10.0f, 10.0f};

    f32 range = max - min;


    vec4 color = {0.5f, 0.5f, 0.5f, 0.5f};

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
    else if (equals(debug->next_hot_item.id, {value}))
    {
        color = hover_color;
    }

    draw_rect(ren, V3(pos), {size.x+button_size.x, size.y}, ui_color);
    vec2 text_pos = {(pos.x+size.x+button_size.x), pos.y};

    TemporaryArena temp_arena = begin_temporary_memory(&debug->arena);

    char *text = PushMemory(&debug->arena, char, 30);
    sprintf_s(text, 30, "%.2f", *value);
    draw_debug_text(ren, text, V3(text_pos));


    button_position.x += (((*value)-min)/range) * size.x;
    draw_rect(ren, V3(button_position), button_size, color);

    if (point_is_inside(debug->mouse_pos,
                        button_position,
                        button_size))
    {
        debug->hot_item.id = {value};
        debug->hot_item.type = INTERACTABLE_TYPE_DRAG;
    }
    else if (point_is_inside(debug->mouse_pos,
                             pos,
                             size))
    {
        debug->hot_item.id = {value};
        debug->hot_item.type = INTERACTABLE_TYPE_CLICK;
    }

    debug->draw_cursor.x += (size.x + text_box_size(ren, text).x) + UI_PADDING;

    end_temporary_memory(&temp_arena);
}

internal void
debug_menu_newline(Renderer* ren, DebugState* debug)
{
    debug->prev_draw_cursor.y -= ren->font_size + UI_PADDING;
    debug->draw_cursor = debug->prev_draw_cursor;
}

internal void
debug_menu_begin(Renderer* ren,
                 DebugState* debug, 
                 vec2 pos, 
                 vec2 size,
                 char* menu_name)
{
    vec4 color = ui_color;
    pos = debug->menu_pos;

    if (equals(debug->interacting_item.id, {menu_name}))
    {
        debug->menu_pos = debug->menu_pos + (debug->mouse_pos - debug->prev_mouse_pos);
        color = clicked_color;
    }
    else if (equals(debug->next_hot_item.id, {menu_name}))
    {
        color = hover_color;
    }

    debug->prev_draw_cursor = pos;
    vec2 header_size = {size.x, ren->font_size + 5.0f};
    vec2 header_pos = {pos.x, pos.y + size.y - header_size.y};
    draw_rect(ren, V3(pos), size, {0.5f, 0.5f, 0.5f, 0.5f});
    draw_rect(ren, V3(header_pos), header_size, color);

    pos = header_pos;
    pos.x += size.x / 2.0f;
    pos.y += 5.0f;
    draw_debug_text(ren, menu_name, V3(pos), TEXT_ALIGN_MIDDLE);

    debug->prev_draw_cursor.y += size.y - 2.0f * header_size.y;
    debug->prev_draw_cursor.x += UI_PADDING;

    if (point_is_inside(debug->mouse_pos,
                        header_pos,
                        header_size))
    {
        debug->hot_item.id = {menu_name};
        debug->hot_item.type = INTERACTABLE_TYPE_DRAG;
    }

    debug->draw_cursor = debug->prev_draw_cursor;
}

internal void
debug_menu_end(Renderer* ren,
              DebugState* debug, 
              vec2 pos, 
              vec2 size,
              char* menu_name)
{
}

