global_variable vec4 ui_color = {0.1f, 0.1f, 0.1f, 1.0f};
global_variable vec4 hover_color = {0.7f, 0.7f, 0.7f, 1.0f};
global_variable vec4 clicked_color = {1.0f, 1.0f, 1.0f, 1.0f};


internal InteractableUiItem*
push_interactable_item(MemoryArena* arena, InteractableUiItemList* ui_item_list)
{
    InteractableUiItem* ui_item = PushMemory(arena, InteractableUiItem);
    if (!ui_item_list->head)
    {
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

internal void 
process_debug_ui_interactions(DebugState* debug, GameInput* input)
{
    InteractableUiItem* ui_item = debug->ui_item_list.head;
    InteractableUiItem hot_item = debug->hot_item;
    InteractableUiItem active_item = debug->active_item;
    while(ui_item)
    {
        if (equals(ui_item->id, active_item.id))
        {
            switch(ui_item->type)
            {
                case INTERACTABLE_UI_ITEM_TYPE_CLICK:
                {
                    debug->active_item = {};
                } break;
                case INTERACTABLE_UI_ITEM_TYPE_DRAG:
                {
                    if (button_released(input->left_mouse_button))
                    {
                        debug->active_item = {};
                    }
                } break;
            }
            break;
        }
        else if (equals(ui_item->id, hot_item.id))
        {
            switch(ui_item->type)
            {
                case INTERACTABLE_UI_ITEM_TYPE_CLICK:
                {
                    if (button_released(input->left_mouse_button))
                    {
                        debug->active_item = (*ui_item);
                    }
                } break;
                case INTERACTABLE_UI_ITEM_TYPE_DRAG:
                {
                    if (button_down(input->left_mouse_button))
                    {
                        debug->active_item = (*ui_item);
                    }
                } break;
            }
            debug->hot_item = {};
        }
        if (point_is_inside(input->mouse.position,
                            ui_item->pos,
                            ui_item->size))
        {
            debug->hot_item = (*ui_item);
            break;
        }
        ui_item = ui_item->next_item;
    }

    debug->mouse_position = input->mouse.position;
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

internal b8
draw_debug_button(Renderer* ren, 
                  DebugState* debug, 
                  vec2 pos, 
                  vec2 size,
                  char* name)
{
    vec4 button_color = ui_color;
    b8 result = false;

    InteractableUiItem* ui_item = push_interactable_item(&debug->arena, &debug->ui_item_list);
    ui_item->pos = pos;
    ui_item->size = size;
    ui_item->id.id = name;
    ui_item->type = INTERACTABLE_UI_ITEM_TYPE_CLICK;


    if (equals(debug->active_item.id, {(void*)name}))
    {
        result = true;
        button_color = clicked_color;
    }
    else if (equals(debug->hot_item.id, {(void*)name}))
    {
        button_color = hover_color;
    }


    draw_rect(ren, V3(pos), size, button_color);
    vec3 text_pos = {
        pos.x + (size.x / 2.0f),
        pos.y + (size.y / 4.0f),
        0.0f,
    };
    draw_debug_text(ren, name, text_pos, TEXT_ALIGN_MIDDLE);



    return result;
}

internal void
draw_debug_slider(Renderer* ren, 
                  DebugState* debug, 
                  vec2 pos, 
                  f32 min,
                  f32 max, 
                  f32* value, 
                  char* var_name)
{
    vec2 button_size = {10.0f, 10.0f};
    vec2 size = {100.0f, 10.0f};
    vec2 position = pos;

    InteractableUiItem* button_item = push_interactable_item(&debug->arena, &debug->ui_item_list);
    InteractableUiItem* full_rect_item = push_interactable_item(&debug->arena, &debug->ui_item_list);

    full_rect_item->pos = position;
    full_rect_item->size = size;
    full_rect_item->id.id = value;
    full_rect_item->type = INTERACTABLE_UI_ITEM_TYPE_CLICK;

    draw_rect(ren, V3(position), {size.x+button_size.x, size.y}, ui_color);
    vec2 text_pos = {(pos.x+size.x+button_size.x), pos.y};

    TemporaryArena temp_arena = begin_temporary_memory(&debug->arena);
    char *text = PushMemory(&debug->arena, char, 30);
    sprintf_s(text, 30, "%.2f", *value);
    draw_debug_text(ren, text, V3(text_pos));

    end_temporary_memory(&temp_arena);

    vec4 color = {0.5f, 0.5f, 0.5f, 0.5f};

    if (equals(debug->active_item.id, {value}))
    {
        if (debug->active_item.type ==
            INTERACTABLE_UI_ITEM_TYPE_DRAG)
        {
            color = clicked_color;
        }
        f32 slider_x = pos.x + (debug->mouse_position.x - pos.x);
        slider_x = CLAMP(slider_x,
                         pos.x,
                         pos.x + size.x);
        f32 update_value = ((slider_x - pos.x)/(size.x));
        if (update_value)
        {
            *value = max * update_value;
        }
        else
        {
            *value = min;
        }
    }
    else if (equals(debug->hot_item.id, {value}))
    {
        color = hover_color;
    }

    position.x += ((*value)/max) * size.x;

    button_item->pos = position;
    button_item->size = button_size;
    button_item->id.id = value;
    button_item->type = INTERACTABLE_UI_ITEM_TYPE_DRAG;


    draw_rect(ren, V3(position), button_size, color);

}

