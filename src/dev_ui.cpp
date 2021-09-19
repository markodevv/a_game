

global_variable Color bg_main_color = {43, 46, 74, 150};
global_variable Color faint_bg_color = {25, 25, 45, 150};
global_variable Color faint_red_color = {144, 55, 33, 150};
global_variable Color faint_redblue_color = {83, 53, 74, 150};
global_variable Color red_color = {232, 69, 69, 150};

#define PADDING 5.0f
#define MAX_NAME_WIDTH 120

#define WINDOW_NO_TITLEBAR (1 << 1)
#define WINDOW_NO_RESIZE (1 << 2)
// WARNING: Macro madness bellow, don't look at it for too long!!

#define UiFloat32Editbox(ui, var, name) \
AnyTypeEditBox(ui, var, (sizeof(*var)/sizeof(f32)), name, f32, false)

#define UiFloat64Editbox(ui, var, name) \
AnyTypeEditBox(ui, var, (sizeof(*var)/sizeof(f64)), name, f64, false)

#define UiInt32Editbox(ui, var, name) \
AnyTypeEditBox(ui, var, (sizeof(*var)/sizeof(i32)), name, i32, true)

#define UiInt8Editbox(ui, var, name) \
AnyTypeEditBox(ui, var, (sizeof(*var)/sizeof(i8)), name, i8, true)

#define UiUInt8Editbox(ui, var, name) \
AnyTypeEditBox(ui, var, (sizeof(*var)/sizeof(u8)), name, u8, true)


#define AnyTypeEditBox(ui, v, num_components, var_name, type, is_int) \
{ \
f32 total_width = (ui->current_window->size.x - MAX_NAME_WIDTH - 2*PADDING) - ((num_components-1) * PADDING); \
type* iterator = (type*)v; \
DrawEditbox(ui, iterator, (*iterator), var_name, V2(total_width/num_components, 20), type, is_int) \
\
for (u32 i = 1; i < num_components; ++i) \
{ \
UiCursorSameline(ui); \
iterator++; \
DrawEditbox(ui, iterator, (*iterator), 0, V2(total_width/num_components, 20), type, is_int) \
} \
} \

#define DrawEditbox(ui, pointer, value, var_name, size, type, is_int) \
{ \
RenderGroup* group = ui->render_group; \
Layer layer = ui->current_window ? ui->current_window->layer : 0; \
if (var_name) \
{ \
UiCursorNewline(ui, size.x + MAX_NAME_WIDTH, size.y); \
} \
else \
{ \
UiCursorNewline(ui, size.x, size.y); \
} \
vec2 pos = ui->draw_cursor; \
Color color = bg_main_color; \
b32 is_editing_box = false; \
if (var_name) \
{ \
if (NameIsTooLong(&ui->font, var_name)) \
{ \
char* temp_name = StringCopy(&ui->temp_arena, var_name); \
TruncateName(temp_name); \
DEBUGDrawText(group,  \
&ui->font, temp_name,  \
V2(pos.x, pos.y + (size.y - ui->font.font_size)),  \
layer + LAYER_FRONT); \
} \
else \
{ \
DEBUGDrawText(group,  \
&ui->font, var_name,  \
V2(pos.x, pos.y + (size.y - ui->font.font_size)),  \
layer + LAYER_FRONT); \
} \
} \
pos.x += MAX_NAME_WIDTH; \
\
\
char* text = GetTextToDraw(&ui->temp_arena, value); \
\
if (IsActive(ui, pointer)) \
{ \
if (ButtonPressed(&ui->input, BUTTON_ESCAPE)) \
{ \
ui->active_item = 0; \
} \
UiProcessTextInput(ui); \
\
if (ButtonPressed(&ui->input, BUTTON_ENTER)) \
{ \
i8 sign = 1; \
if (ui->text_input_buffer[0] == '-') \
{ \
for (u32 i = 0; i < ui->text_insert_index; ++i) \
{ \
ui->text_input_buffer[i] = ui->text_input_buffer[i+1]; \
} \
sign = -1; \
} \
\
type number = 0; \
if (is_int) \
{ \
number = (type)atoi(ui->text_input_buffer); \
} \
else \
{ \
number = (type)atof(ui->text_input_buffer); \
} \
*pointer = number * sign; \
ui->active_item = 0;\
} \
\
is_editing_box = true; \
DEBUGDrawText(group, &ui->font, ui->text_input_buffer, V2(pos.x, pos.y + (size.y - ui->font.font_size)), layer + LAYER_FRONT); \
ui->text_input_buffer[ui->text_insert_index] = '\0'; \
\
f32 cursor_x = pos.x + GetTextPixelWidth(&ui->font, ui->text_input_buffer); \
PushQuad(group, V2(cursor_x, pos.y), V2(3, size.y), faint_red_color, layer + LAYER_FRONT); \
} \
else \
{ \
if (IsHot(ui, pointer)) \
{ \
color.r *= 2; \
color.g *= 2; \
color.b *= 2; \
if (ButtonPressed(&ui->input, BUTTON_MOUSE_LEFT)) \
{ \
SetActive(ui, pointer); \
FillTextBuffer(ui, value); \
ui->editbox_value_to_set = iterator; \
ui->editbox_value_to_set_size = sizeof(type); \
ui->editbox_value_is_int = is_int; \
} \
} \
DEBUGDrawText(group,  \
&ui->font, \
text,  \
V2(pos.x + (size.x / 2),  \
pos.y + (size.y - ui->font.font_size)), \
layer + LAYER_FRONT, \
TEXT_ALIGN_MIDDLE); \
} \
\
PushQuad(group, pos, size, color, layer + LAYER_MID); \
\
if (WindowIsFocused(ui, ui->current_window)) \
{ \
if (PointInsideRect(ui->input.mouse.position, pos, size)) \
{ \
SetHot(ui, pointer); \
} \
} \
} \


internal void
UiProcessTextInput(DevUI* ui)
{
    GameInput* input = &ui->input;
    
    if (input->character != '\0')
    {
        char c = input->character;
        if ((c >= 48 && c < 58) ||
            c == 46 ||
            c == 43 ||
            c == 45)
        {
            ui->text_input_buffer[ui->text_insert_index] = input->character;
            ui->text_input_buffer[ui->text_insert_index + 1] = '\0';
            ++ui->text_insert_index;
        }
    }
    if (ButtonPressed(input, BUTTON_BACKSPACE) && 
        ModifierPressed(input, SHIFT_MODIF))
    {
        ui->text_insert_index = 0;
        ui->text_input_buffer[0] = '\0';
    }
    else if (ButtonPressed(input, BUTTON_BACKSPACE) &&
             ui->text_insert_index)
    {
        --ui->text_insert_index;
        ui->text_input_buffer[ui->text_insert_index] = '\0';
    }
}

internal b32 
WindowIsFocused(DevUI* ui, UiWindow* window)
{
    return ui->focused_window == window;
}

internal void
SetWindowFocus(DevUI* ui, UiWindow* window)
{
    ui->top_layer += 20;
    window->layer = ui->top_layer;
    ui->focused_window = window;
}

internal UiWindow*
GetWindow(DevUI* ui, char* id)
{
    u64 hash = 5381;
    i32 c;
    
    char* key = id;
    while((c = *key++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    
    u16 hash_index = hash % ArrayCount(ui->window_table);
    
    UiWindow** ui_window = ui->window_table + hash_index;
    
    UiWindow* result = 0;
    for (UiWindow* window = (*ui_window);
         window;
         window = window->next)
    {
        if (StringMatch(window->id, id))
        {
            result = window;
            return result;
        }
    }
    
    return result;
    
}

internal UiWindow*
GetOrCreateWindow(DevUI* ui, char* id)
{
    u64 hash = 5381;
    i32 c;
    
    char* key = id;
    while((c = *key++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    
    u16 hash_index = hash % ArrayCount(ui->window_table);
    
    UiWindow** ui_window = ui->window_table + hash_index;
    
    UiWindow* result = 0;
    for (UiWindow* window = (*ui_window);
         window;
         window = window->next)
    {
        if (StringMatch(window->id, id))
        {
            result = window;
            return result;
        }
    }
    
    if (!result)
    {
        result = PushMemory(&ui->arena, UiWindow);
        result->next = *ui_window;
        result->id = StringCopy(&ui->arena, id);
        *ui_window = result;
    }
    
    return result;
}

internal void
SetWindowPosition(DevUI* ui, vec2 position, char* window_name)
{
    GetOrCreateWindow(ui, window_name)->position = position;
}

internal void
SetWindowSize(DevUI* ui, vec2 size, char* window_name)
{
    GetOrCreateWindow(ui, window_name)->size = size;
}

internal b32
IsHot(DevUI* ui, void* id)
{
    return ui->next_hot_item == id;
}


internal b32
IsActive(DevUI* ui, void* id)
{
    return ui->active_item == id;
}

internal void
SetActive(DevUI* ui, void* id)
{
    ui->hot_item = 0;
    ui->active_item = id;
}

internal void
SetHot(DevUI* ui, void* id)
{
    
    ui->hot_item = id;
}


internal inline void
UiCursorSameline(DevUI* ui)
{
    ui->is_newline = false;
}

internal inline vec2
UiCursorNewline(DevUI* ui, f32 w, f32 h)
{
    if (ui->is_newline)
    {
        ui->draw_cursor.x = ui->current_window->position.x + PADDING;
        ui->draw_cursor.y -= h + PADDING;
    }
    else
    {
        ui->draw_cursor.x += w + PADDING;
    }
    ui->is_newline = true;
    
    return ui->draw_cursor;
}

internal b32
NameIsTooLong(Font* font, char* name)
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
TruncateName(char* name)
{
    u32 len = StringLength(name);
    name[len-1] = '\0';
    name[len-2] = '.';
    name[len-3] = '.';
}

internal char*
GetTextToDraw(MemoryArena* arena, u64 value)
{
    char* result = PushMemory(arena, char, 128);
    snprintf(result, 128, "%llu", value);
    
    return result;
}

internal char*
GetTextToDraw(MemoryArena* arena, f64 value)
{
    char* result = PushMemory(arena, char, 128);
    snprintf(result, 128, "%.2f", value);
    
    return result;
}

internal char*
GetTextToDraw(MemoryArena* arena, i64 value)
{
    char* result = PushMemory(arena, char, 128);
    snprintf(result, 128, "%lli", value);
    
    return result;
}

internal char*
GetTextToDraw(MemoryArena* arena, u32 value)
{
    char* result = PushMemory(arena, char, 128);
    snprintf(result, 128, "%d", value);
    
    return result;
}

internal char*
GetTextToDraw(MemoryArena* arena, i32 value)
{
    char* result = PushMemory(arena, char, 128);
    snprintf(result, 128, "%d", value);
    
    return result;
}

internal char*
GetTextToDraw(MemoryArena* arena, f32 value)
{
    char* result = PushMemory(arena, char, 128);
    snprintf(result, 128, "%.2f", value);
    
    return result;
}

internal void
DeleteTrailingZeros(DevUI* ui)
{
    ui->text_insert_index = StringLength(ui->text_input_buffer);
    
    for (u32 i = ui->text_insert_index-1;;--i)
    {
        if (ui->text_input_buffer[i] == '0')
        {
            --ui->text_insert_index;
        }
        else
        {
            break;
        }
    }
    
    ui->text_input_buffer[ui->text_insert_index] = '\0';
}
internal void
FillTextBuffer(DevUI* ui, f64 value)
{
    snprintf(ui->text_input_buffer, 
             ArrayCount(ui->text_input_buffer), 
             "%f",
             value);
    DeleteTrailingZeros(ui);
}

internal void
FillTextBuffer(DevUI* ui, f32 value)
{
    snprintf(ui->text_input_buffer, 
             ArrayCount(ui->text_input_buffer), 
             "%f",
             value);
    DeleteTrailingZeros(ui);
}

internal void
FillTextBuffer(DevUI* ui, i64 value)
{
    snprintf(ui->text_input_buffer, 
             ArrayCount(ui->text_input_buffer), 
             "%lli",
             value);
    ui->text_insert_index = StringLength(ui->text_input_buffer);
    ui->text_input_buffer[ui->text_insert_index] = '\0';
}

internal void
FillTextBuffer(DevUI* ui, u64 value)
{
    snprintf(ui->text_input_buffer, 
             ArrayCount(ui->text_input_buffer), 
             "%llu",
             value);
    ui->text_insert_index = StringLength(ui->text_input_buffer);
    ui->text_input_buffer[ui->text_insert_index] = '\0';
}

internal void
FillTextBuffer(DevUI* ui, u32 value)
{
    snprintf(ui->text_input_buffer, 
             ArrayCount(ui->text_input_buffer), 
             "%u",
             value);
    ui->text_insert_index = StringLength(ui->text_input_buffer);
    ui->text_input_buffer[ui->text_insert_index] = '\0';
}

internal void
FillTextBuffer(DevUI* ui, i32 value)
{
    snprintf(ui->text_input_buffer, 
             ArrayCount(ui->text_input_buffer), 
             "%i",
             value);
    ui->text_insert_index = StringLength(ui->text_input_buffer);
    ui->text_input_buffer[ui->text_insert_index] = '\0';
}

internal Font
UiLoadFont(MemoryArena* arena, Platform* platform, Assets* assets, char* font_path, i32 font_size)
{
    Font font = {};
    font.sprite_ids = PushMemory(arena, SpriteID, NUM_ASCII);
    font.char_metrics = PushMemory(arena, CharMetric, NUM_ASCII);
    font.num_chars = NUM_ASCII;
    
    
    font.font_sprite_id = DEBUGCreateFontSprite(assets, 512, 512, 1);
    font.font_size = font_size;
    
    Sprite* font_sprite = GetSprite(assets, font.font_sprite_id);
    FileResult font_file = platform->ReadEntireFile(font_path);
    
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
            LogM("Failed to bake font map\n");
        }
        
        for(u8 i = 0; i < NUM_ASCII; ++i)
        {
            stbtt_aligned_quad q;
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
            
            SpriteID id = AddSubsprite(assets, font.font_sprite_id);
            Sprite* subsprite = GetSprite(assets, id);
            font.sprite_ids[i] = id;
            subsprite->name = "Char";
            
            subsprite->uvs[0] = {q.s1, q.t1};
            subsprite->uvs[1] = {q.s1, q.t0};
            subsprite->uvs[2] = {q.s0, q.t0};
            subsprite->uvs[3] = {q.s0, q.t1};
            
            
            // TODO:
            subsprite->main_sprite = font.font_sprite_id;
        }
        platform->FreeFile(&font_file);
    }
    else
    {
        LogM("Failed to load font [%s] file.\n", font_path);
    }
    
    
    return font;
}

// TODO: title is not a good pointer for id
internal void
UiSubmenuTitlebar(DevUI* ui, vec2 position, vec2 size, char* title)
{
    Color color = bg_main_color;
    Layer layer = ui->current_window ? ui->current_window->layer : 0;
    
    if (IsHot(ui, title))
    {
        color = faint_red_color;
        if (ButtonPressed(&ui->input, BUTTON_MOUSE_LEFT))
        {
            color = faint_redblue_color;
            b32* active = &ui->sub_menus[ui->sub_menu_index].is_active;
            *active = !(*active);
        }
    }
    
    if (WindowIsFocused(ui, ui->current_window))
    {
        if (PointInsideRect(ui->input.mouse.position,
                            position,
                            size))
        {
            SetHot(ui, title);
        }
    }
    
    PushQuad(ui->render_group, position, size, color, layer + LAYER_MID);
    DEBUGDrawText(ui->render_group, 
                  &ui->font, 
                  title, 
                  V2(position.x + (size.x / 2),
                     position.y + 2),
                  layer + LAYER_FRONT,
                  TEXT_ALIGN_MIDDLE);
}

internal void
ReadUiConfig(Platform* platform, DevUI* ui)
{
    FileResult file = platform->ReadEntireFile("./config.ui");
    
    if (ValidFile(&file))
    {
        UiFileHeader* header = (UiFileHeader*)file.data;
        u8* iterator = (u8*)(header + 1);
        
        char window_name[128];
        for (u32 i = 0; i < header->window_count; ++i)
        {
            WindowSerializeData* window_data = (WindowSerializeData*)iterator;
            iterator += sizeof(WindowSerializeData);
            StringCopy(window_name, window_data->name, window_data->name_size);
            
            UiWindow* window = GetWindow(ui, window_name);
            if (window)
            {
                window->size = window_data->size;
                window->position = window_data->position;
            }
        }
        platform->FreeFile(&file);
    }
    else
    {
        LogM("Failed to read ui config file\n");
    }
}

internal void
WriteUiConfig(Platform* platform, DevUI* ui)
{
    u8* file_memory = PushMemory(&ui->temp_arena, u8, Megabytes(10));
    UiFileHeader* file_header = (UiFileHeader*)file_memory;
    u8* iterator = file_memory + sizeof(UiFileHeader);
    
    u32 file_size = sizeof(UiFileHeader);
    u32 window_count = 0;
    
    for (u32 i = 0; i < ArrayCount(ui->window_table); ++i)
    {
        for (UiWindow* window = ui->window_table[i]; 
             window; 
             window = window->next)
        {
            u32 name_size = StringLength(window->id) * sizeof(char);
            
            WindowSerializeData* window_data = (WindowSerializeData*)iterator;
            
            window_data->name_size = name_size;
            StringCopy(window_data->name, window->id);
            window_data->position = window->position;
            window_data->size = window->size;
            
            iterator += sizeof(WindowSerializeData);
            file_size += sizeof(WindowSerializeData);
            
            window_count++;
        }
    }
    
    file_header->window_count = window_count;
    
    b32 success = platform->WriteEntireFile("./config.ui", file_size, file_memory);
    
    if (!success)
    {
        LogM("Failed to write ui config file!\n");
    }
}

internal void
DevUiStart(DevUI* ui, GameInput* input, Assets* assets, Renderer2D* ren)
{
    if (ui->temp_memory.arena)
    {
        EndTemporaryMemory(&ui->temp_memory);
    }
    ui->temp_memory = BeginTemporaryMemory(&ui->temp_arena);
    
    Assert(ui->temp_memory.arena);
    
    ui->prev_mouse_pos = ui->input.mouse.position;
    ui->input = *input;
    ui->screen_width = ren->screen_width;
    ui->screen_height = ren->screen_height;
    ui->current_window = 0;
    ui->sub_menu_index = 0;
    
    ui->next_hot_item = ui->hot_item;
    ui->hot_item = 0;
    
    
    
    if (ButtonPressed(input, BUTTON_MOUSE_LEFT))
    {
        UiWindow** windows =  PushMemory(&ui->temp_arena, 
                                         UiWindow*, 
                                         ArrayCount(ui->window_table));
        u32 len = 0;
        
        for (u32 i = 0; i < ArrayCount(ui->window_table); ++i)
        {
            for (UiWindow* window = ui->window_table[i]; 
                 window; 
                 window= window->next)
            {
                windows[len] = window;
                len++;
            }
        }
        
        LogM("Number of windows %i\n", len);
        
        // TODO: better sort
        // Sort by layer
        for (i32 i = 0; i < (i32)len; ++i)
        {
            UiWindow* win = windows[i];
            
            i32 j = i - 1;
            while (j >= 0 && windows[j]->layer < win->layer)
            {
                windows[j+1] = windows[j];
                --j;
            }
            windows[j+1] = win;
        }
        
        
        // Top window should always be focused
        UiWindow* focused_window = windows[0];
        if (PointInsideRect(input->mouse.position,
                            focused_window->position,
                            focused_window->size))
        {
            return;
        }
        
        for (u32 i = 1; i < len; ++i)
        {
            UiWindow* window = windows[i];
            if (PointInsideRect(input->mouse.position,
                                window->position,
                                window->size))
            {
                SetWindowFocus(ui, window);
                break;
            }
        }
    }
}


internal void
PushWindow(DevUI* ui, UiWindow* window)
{
    StackedWindow* sw = ui->window_stack + ui->num_stacked_windows;
    sw->window = window;
    sw->previous_draw_cursor = ui->draw_cursor;
    
    ++ui->num_stacked_windows;
    Assert(ui->num_stacked_windows < ArrayCount(ui->window_stack));
}

internal StackedWindow*
PopWindow(DevUI* ui)
{
    --ui->num_stacked_windows;
    return ui->window_stack + ui->num_stacked_windows;
}

internal void
UiWindowResizeThingy(DevUI* ui, UiWindow* window)
{
    vec2 resize_thingy_size = V2(10, 10);
    vec2 resize_thingy_pos = V2(window->position.x + window->size.x - resize_thingy_size.x,
                                window->position.y);
    
    Color color = bg_main_color;
    
    if (IsActive(ui, window))
    {
        if (ButtonDown(&ui->input, BUTTON_MOUSE_LEFT))
        {
            vec2 mouse_delta = (ui->input.mouse.position - ui->prev_mouse_pos);
            window->size.x += mouse_delta.x;
            window->size.y -= mouse_delta.y;
            window->position.y += mouse_delta.y;
            
            resize_thingy_pos = V2(window->position.x + window->size.x - resize_thingy_size.x,
                                   window->position.y);
            
        }
        else
        {
            SetActive(ui, 0);
        }
    }
    else if (IsHot(ui, window))
    {
        if (ButtonPressed(&ui->input, BUTTON_MOUSE_LEFT))
        {
            SetActive(ui, window);
        }
        color = faint_red_color;
    }
    
    if (WindowIsFocused(ui, window))
    {
        if (PointInsideRect(ui->input.mouse.position,
                            resize_thingy_pos,
                            resize_thingy_size))
        {
            SetHot(ui, window);
        }
    }
    
    PushQuad(ui->render_group, resize_thingy_pos, resize_thingy_size, color, window->layer + LAYER_FRONT);
}

internal void
UiWindowBegin(DevUI* ui, char* title, vec2 pos = V2(0), vec2 size = V2(400, 500), u32 flags = 0)
{
    UiWindow* window = GetOrCreateWindow(ui, title);
    
    Assert(window);
    
    if (ui->current_window)
    {
        PushWindow(ui, ui->current_window);
        
    }
    
    ui->current_window = window;
    Layer layer = window->layer;
    
    
    if (window->size.x <= 0 || window->size.y <= 0)
    {
        window->position = V2(RandomRange(100, 200),
                              RandomRange(100, 200));
        window->size = size;
    }
    
    
    Color color = faint_red_color;
    
    if (IsActive(ui, title))
    {
        if (ButtonDown(&ui->input, BUTTON_MOUSE_LEFT))
        {
            color = faint_red_color;
            vec2 mouse_delta = (ui->input.mouse.position - ui->prev_mouse_pos);
            window->position = window->position + mouse_delta;
        }
        else
        {
            SetActive(ui, 0);
        }
    }
    else if (IsHot(ui, title))
    {
        color = red_color;
        if (ButtonPressed(&ui->input, BUTTON_MOUSE_LEFT))
        {
            SetActive(ui, title);
        }
    }
    
    // we start from top
    ui->draw_cursor = V2(window->position.x,
                         window->position.y + window->size.y);
    
    if ((flags & WINDOW_NO_RESIZE) != WINDOW_NO_RESIZE)
    {
        UiWindowResizeThingy(ui, window);
    }
    
    
    PushQuad(ui->render_group, 
             ui->current_window->position,
             ui->current_window->size,
             faint_bg_color,
             ui->current_window->layer + LAYER_BACKMID);
    
    
    if ((flags & WINDOW_NO_TITLEBAR) != WINDOW_NO_TITLEBAR)
    {
        vec2 titlebar_size = V2((f32)window->size.x, (f32)ui->font.font_size);
        ui->draw_cursor.y -= titlebar_size.y;
        vec2 titlebar_pos = ui->draw_cursor;
        
        
        // NOTE: there must be a focused window so i check here because on start there is
        // no focused window and set it to first UiWindowBegin window
        
        if (!ui->focused_window)
        {
            SetWindowFocus(ui, window);
        }
        
        
        if (WindowIsFocused(ui, window))
        {
            if (PointInsideRect(ui->input.mouse.position,
                                titlebar_pos,
                                titlebar_size))
            {
                SetHot(ui, title);
            }
        }
        
        PushQuad(ui->render_group, titlebar_pos, titlebar_size, color, layer + LAYER_MID);
        DEBUGDrawText(ui->render_group, 
                      &ui->font,
                      title, 
                      V2(titlebar_pos.x + (titlebar_size.x / 2),
                         titlebar_pos.y + 4.0f),
                      layer + LAYER_FRONT,
                      TEXT_ALIGN_MIDDLE);
    }
    
}

internal void
UiWindowEnd(DevUI* ui)
{
    if (ui->num_stacked_windows > 0)
    {
        StackedWindow* sw = PopWindow(ui);
        ui->current_window = sw->window;
        ui->draw_cursor = sw->previous_draw_cursor;
    }
    else
    {
        ui->current_window = 0;
    }
}

internal void
UiFps(DevUI* ui, f32 fps)
{
    RenderGroup* group = ui->render_group;
    char* fps_text = "%.2f fps";
    char out[32];
    sprintf(out, fps_text, fps);
    vec2 position = V2(0.0f, (f32)ui->screen_height - ui->font.font_size);
    
    DEBUGDrawText(group, &ui->font, out, position, 1 << 10);
}



internal void
UiCheckbox(DevUI* ui, b32* toggle_var, char* var_name)
{
    RenderGroup* group = ui->render_group;
    
    vec2 size = V2(20);
    vec2 pos = UiCursorNewline(ui, size.x, size.y);
    Layer layer = ui->current_window ? ui->current_window->layer : 0;
    
    if (NameIsTooLong(&ui->font, var_name))
    {
        var_name = StringCopy(&ui->temp_arena, var_name);
        TruncateName(var_name);
    }
    
    DEBUGDrawText(group,
                  &ui->font, 
                  var_name, 
                  V2(pos.x, pos.y + (size.y - ui->font.font_size)),
                  layer + LAYER_FRONT);
    pos.x += MAX_NAME_WIDTH;
    
    PushQuad(group, pos, size, bg_main_color, layer + LAYER_BACKMID);
    
    Color check_color = {100, 100, 100, 255};
    
    if (IsActive(ui, toggle_var))
    {
        ui->active_item = 0;
    }
    else if (IsHot(ui, toggle_var))
    {
        check_color = faint_red_color;
        if (ButtonPressed(&ui->input, BUTTON_MOUSE_LEFT))
        {
            SetActive(ui, toggle_var);
            *toggle_var = !(*toggle_var);
        }
    }
    
    if (*toggle_var)
    {
        check_color.r *= 2;
        check_color.g *= 2;
        check_color.b *= 2;
    }
    
    PushQuad(group, pos, size, bg_main_color, layer + LAYER_BACKMID);
    size -= 8;
    pos += 4;
    PushQuad(group, pos, size, check_color, layer + LAYER_BACKMID);
    
    if (WindowIsFocused(ui, ui->current_window))
    {
        if (PointInsideRect(ui->input.mouse.position,
                            pos,
                            size))
        {
            SetHot(ui, toggle_var);
        }
    }
}

internal b32
UiButton(DevUI* ui, char* name, vec2 pos = V2(0), vec2 size = V2(80, 40))
{
    RenderGroup* group = ui->render_group;
    
    
    if (ui->current_window)
        pos = UiCursorNewline(ui, size.x, size.y);
    
    
    Color button_color = bg_main_color;
    b32 result = false;
    Layer layer = ui->current_window ? ui->current_window->layer : 0;
    
    
    if (IsActive(ui, name))
    {
        if (ButtonReleased(&ui->input, BUTTON_MOUSE_LEFT))
        {
            result = true;
            ui->active_item = 0;
        }
        button_color = faint_red_color;
    }
    else if (IsHot(ui, name))
    {
        if (ButtonPressed(&ui->input, BUTTON_MOUSE_LEFT))
        {
            SetActive(ui, name);
        }
        
        button_color.r *= 2;
        button_color.g *= 2;
        button_color.b *= 2;
    }
    
    PushQuad(group, pos, size, button_color, layer + LAYER_MID);
    vec2 text_pos = V2(
                       pos.x + (size.x / 2.0f),
                       pos.y + (size.y / 2.0f) - (ui->font.font_size / 4.0f)
                       );
    DEBUGDrawText(group, &ui->font, name, text_pos, layer + LAYER_FRONT, TEXT_ALIGN_MIDDLE);
    
    
    if (WindowIsFocused(ui, ui->current_window))
    {
        if (PointInsideRect(ui->input.mouse.position,
                            pos,
                            size))
        {
            SetHot(ui, name);
        }
    }
    
    
    return result;
}


internal void
UiSilder(DevUI* ui, 
         f32 min,
         f32 max, 
         f32* value, 
         char* var_name)
{
    RenderGroup* group = ui->render_group;
    
    vec2 bar_size = V2(ui->current_window->size.x - MAX_NAME_WIDTH - 2*PADDING, 20.0f);
    vec2 pos = UiCursorNewline(ui, bar_size.x + MAX_NAME_WIDTH, bar_size.y);
    
    Layer layer = ui->current_window ? ui->current_window->layer : 0;
    
    if (NameIsTooLong(&ui->font, var_name))
    {
        var_name = StringCopy(&ui->temp_arena, var_name);
        TruncateName(var_name);
    }
    DEBUGDrawText(group,
                  &ui->font, 
                  var_name, 
                  V2(pos.x, pos.y + (bar_size.y - ui->font.font_size)), 
                  layer + LAYER_FRONT);
    
    pos.x += MAX_NAME_WIDTH;
    
    vec2 button_position = pos;
    vec2 button_size = V2(10, bar_size.y);
    button_position.y -= (button_size.y - bar_size.y) / 2.0f;
    
    f32 range = max - min;
    
    
    Color button_color = faint_redblue_color;
    
    if (IsActive(ui, value))
    {
        if (ButtonDown(&ui->input, BUTTON_MOUSE_LEFT))
        {
            button_color = red_color;
            f32 slide_amount = (ui->input.mouse.position.x - pos.x);
            slide_amount = Clamp(slide_amount, 0.0f, bar_size.x - button_size.x);
            
            f32 update_value = slide_amount / (bar_size.x - button_size.x);
            
            *value = (range * update_value) + min;
        }
        else
        {
            ui->active_item = 0;
        }
    }
    else if (IsHot(ui, value))
    {
        button_color = faint_red_color;
        if (ButtonPressed(&ui->input, BUTTON_MOUSE_LEFT))
        {
            SetActive(ui, value);
        }
    }
    
    PushQuad(group, pos, bar_size, bg_main_color, layer + LAYER_MID);
    
    
    vec2 text_pos = V2(pos.x+(bar_size.x/2), pos.y + (bar_size.y - ui->font.font_size));
    char text[32];
    snprintf(text, 32, "%.2f", *value);
    DEBUGDrawText(ui->render_group, &ui->font, text, text_pos, layer + LAYER_FRONT, TEXT_ALIGN_MIDDLE);
    
    button_position.x += (((*value)-min)/range) * (bar_size.x - button_size.x);
    PushQuad(group, button_position, button_size, button_color, layer + LAYER_FRONT);
    
    if (WindowIsFocused(ui, ui->current_window))
    {
        if (PointInsideRect(ui->input.mouse.position, button_position, button_size))
        {
            SetHot(ui, value);
        }
    }
    
}

internal void
HueSlider(DevUI* ui, Colorpicker* cp)
{
    
    Color cursor_color = NewColor(255);
    vec2 size = V2(20, 200);
    UiCursorNewline(ui, size.x, size.y);
    vec2 pos = ui->draw_cursor;
    
    if (IsActive(ui, &cp->hue))
    {
        if (ButtonDown(&ui->input, BUTTON_MOUSE_LEFT))
        {
            cursor_color.a = 180;
            cp->hue = (ui->input.mouse.position.y - pos.y) / size.y;
            cp->hue = Clamp(cp->hue, 0.0f, 1.0f);
        }
        else
        {
            ui->active_item = 0;
        }
    }
    else if (IsHot(ui, &cp->hue))
    {
        if (ButtonPressed(&ui->input, BUTTON_MOUSE_LEFT))
        {
            SetActive(ui, &cp->hue);
        }
    }
    Shader* hue_shader = GetShader(ui->render_group->assets, SHADER_ID_HUE_QUAD);
    vec2 cursor_pos = V2(pos.x, pos.y + (cp->hue * size.y));
    
    SetShaderUniform(hue_shader, "u_size", V2(20, 200), vec2);
    SetShaderUniform(hue_shader, "u_position", pos, vec2);
    
    PushQuad(ui->render_group, 
             pos,
             size,
             NewColor(100, 0, 40, 255),
             ui->current_window->layer + LAYER_FRONT,
             0,
             SHADER_ID_HUE_QUAD);
    
    PushQuad(ui->render_group,
             cursor_pos,
             V2(20, 4),
             cursor_color,
             ui->current_window->layer + LAYER_FRONT + 10.0f,
             0,
             SHADER_ID_NORMAL);
    
    if (WindowIsFocused(ui, ui->current_window))
    {
        if (PointInsideRect(ui->input.mouse.position, pos, size))
        {
            SetHot(ui, &cp->hue);
        }
    }
    
    
}

internal void
HsbPicker(DevUI* ui, Colorpicker* cp)
{
    vec2 pos = ui->draw_cursor;
    vec2 size = V2(200, 200);
    Color cursor_color = NewColor(255);
    // TODO: this should use ui_same_line() API
    pos.x += 40;
    
    if (IsActive(ui, &cp->saturation))
    {
        if (ButtonDown(&ui->input, BUTTON_MOUSE_LEFT))
        {
            cursor_color.a = 180;
            cp->saturation = (ui->input.mouse.position.x - pos.x) / size.x;
            cp->brightness = (ui->input.mouse.position.y - pos.y) / size.y;
            
            cp->saturation = Clamp(cp->saturation, 0.0f, 1.0f);
            cp->brightness = Clamp(cp->brightness, 0.0f, 1.0f);
        }
        else
        {
            ui->active_item = 0;
        }
    }
    else if (IsHot(ui, &cp->saturation))
    {
        if (ButtonPressed(&ui->input, BUTTON_MOUSE_LEFT))
        {
            SetActive(ui, &cp->saturation);
        }
    }
    
    vec2 cursor_pos = pos + (size * V2(cp->saturation, cp->brightness));
    
    Shader* sb_shader = GetShader(ui->render_group->assets, SHADER_ID_SB_QUAD);
    SetShaderUniform(sb_shader, "u_size", V2(200, 200), vec2);
    SetShaderUniform(sb_shader, "u_position", pos, vec2);
    SetShaderUniform(sb_shader, "u_hue", cp->hue, f32);
    
    PushQuad(ui->render_group, 
             pos,
             size,
             NewColor(255),
             ui->current_window->layer + LAYER_FRONT,
             0,
             SHADER_ID_SB_QUAD);
    
    PushQuad(ui->render_group, 
             cursor_pos,
             V2(4.0f),
             cursor_color,
             ui->current_window->layer + LAYER_FRONT + 10.0f,
             0,
             SHADER_ID_NORMAL);
    
    if (WindowIsFocused(ui, ui->current_window))
    {
        if (PointInsideRect(ui->input.mouse.position, pos, size))
        {
            SetHot(ui, &cp->saturation);
        }
    }
    
}

internal Color 
HsbToRgb(vec3 c)
{
    vec3 rgb = Clamp_V3(Abs_V3(Fmod_V3(V3(0.0f, 4.0f, 2.0f) + c.x * 6.0f,
                                       6.0f) - 3.0f) - 1.0f,
                        0.0f,
                        1.0f );
    rgb = rgb*rgb*(rgb * -2.0f + 3.0f);
    
    c =  vec3_mix(V3(1.0f), rgb, c.y) * c.z;
    Color result;
    result.r = c.x * 255;
    result.g = c.y * 255;
    result.b = c.z * 255;
    result.a = 255;
    
    return result;
}

internal Colorpicker*
GetColorpicker(DevUI* ui, void* key)
{
    u64 hash = ((u64)key) >> 3;
    
    u16 hash_index = hash % ArrayCount(ui->colorpickers);
    
    Colorpicker* cp = ui->colorpickers + hash_index;
    
    do
    {
        if (cp->key == key)
        {
            break;
        }
        
        if (!(cp->next) && !(cp->key))
        {
            cp->key = key;
        }
        
    } while(true);
    
    
    return cp;
}

internal void
UiColorpicker(DevUI* ui, Color* var, char* var_name)
{
    vec2 size = V2(ui->current_window->size.x - MAX_NAME_WIDTH - 2*PADDING,
                   ui->font.font_size);
    Color color = *var;
    
    Layer layer = ui->current_window ? ui->current_window->layer : 0;
    vec2 pos = UiCursorNewline(ui, size.x, size.y);
    
    
    if (NameIsTooLong(&ui->font, var_name))
    {
        var_name = StringCopy(&ui->temp_arena, var_name);
        TruncateName(var_name);
    }
    
    DEBUGDrawText(ui->render_group, 
                  &ui->font, 
                  var_name, 
                  V2(pos.x, pos.y + (size.y - ui->font.font_size)), 
                  layer + LAYER_FRONT);
    pos.x += MAX_NAME_WIDTH;
    
    PushQuad(ui->render_group, pos, size, color, layer + LAYER_MID);
    
    
    UiWindow* colorpicker_window = GetOrCreateWindow(ui, "Color Picker");
    Colorpicker* colorpicker = GetColorpicker(ui, (void*)var);
    
    if (ButtonPressed(&ui->input, BUTTON_MOUSE_LEFT))
    {
        if (!colorpicker->is_active)
        {
            if (WindowIsFocused(ui, ui->current_window))
            {
                if (PointInsideRect(ui->input.mouse.position, pos, size))
                {
                    vec2 window_size = V2(260 + 2*PADDING, 300);
                    vec2 window_pos = ui->input.mouse.position;
                    SetWindowFocus(ui, colorpicker_window);
                    
                    if ((window_pos.y + window_size.y) >= ui->screen_height ||
                        (window_pos.x + window_size.x) >= ui->screen_width)
                    {
                        window_pos.x = ui->screen_width - window_size.x;
                        window_pos.y = ui->screen_height - window_size.y;
                    }
                    SetWindowPosition(ui, window_pos, "Color Picker");
                    SetWindowSize(ui, window_size, "Color Picker");
                    
                    colorpicker->color = *var;
                    colorpicker->is_active = true;
                }
            }
        }
        else
        {
            if (!PointInsideRect(ui->input.mouse.position, 
                                 colorpicker_window->position, colorpicker_window->size))
            {
                colorpicker->is_active = false;
            }
        }
    }
    
    if (colorpicker->is_active)
    {
        
        
        UiWindowBegin(ui, "Color Picker", 
                      colorpicker_window->position, 
                      colorpicker_window->size,
                      WINDOW_NO_RESIZE);
        
        
        
        HueSlider(ui, colorpicker);
        HsbPicker(ui, colorpicker);
        
        *var = HsbToRgb(V3(colorpicker->hue,
                           colorpicker->saturation,
                           colorpicker->brightness));
        
        
        
        UiUInt8Editbox(ui, var, "Color");
        
        UiWindowEnd(ui);
        
    }
    
}

internal void
UiProfilerWindow(ProfileEntry* entries, DevUI* ui)
{
    
#ifdef GAME_PROFILE
    UiWindowBegin(ui, "Profiler");
    char buffer[64];
    
    UiWindow* window = GetOrCreateWindow(ui, "Profiler");
    ui->draw_cursor.y -= ui->font.font_size;
    ui->draw_cursor.x += PADDING;
    
    for (ProfileEntry* entry = entries;
         entry;
         entry = entry->next)
    {
        f64 ms_time = (entry->elapsed_sec * 1000.0f);
        snprintf(buffer, ArrayCount(buffer), "%s - %.2f ms", entry->name, ms_time);
        DEBUGDrawText(ui->render_group, &ui->font, buffer, ui->draw_cursor, window->layer + LAYER_FRONT, TEXT_ALIGN_LEFT);
        
        ui->draw_cursor.y -= ui->font.font_size;
    }
    UiWindowEnd(ui);
#endif
    
}

internal void
DevUiInit(DevUI* dev_ui, MemoryArena* arena, Renderer2D* renderer, Platform* platform, Assets* assets)
{
    SubArena(arena, &dev_ui->arena, Megabytes(64));
    SubArena(arena, &dev_ui->temp_arena, Megabytes(32));
    
    dev_ui->font = UiLoadFont(&dev_ui->arena,
                              platform,
                              assets,
                              "../assets/fonts/consola.ttf",
                              14);
    
    for (u32 i = 0; i < ArrayCount(dev_ui->sub_menus); ++i)
    {
        dev_ui->sub_menus[i].is_active = true;
    }
    
    mat4 projection = Mat4Orthographic(renderer->screen_width,
                                       renderer->screen_height);
    
    dev_ui->render_group = CreateRenderGroup(&dev_ui->arena,
                                             projection,
                                             CreateCamera(Vec3Up(), Vec3Forward(), V3(0.0f, 0.0f, 9000.0f)),
                                             renderer, 
                                             assets,
                                             Megabytes(48));
    dev_ui->top_layer = 4069;
}



internal b32
UiSubmenu(DevUI* ui, char* title)
{
    ui->draw_cursor.y -=  ui->font.font_size + PADDING;
    ui->draw_cursor.x = ui->current_window->position.x + PADDING;
    
    
    UiSubmenuTitlebar(ui, 
                      ui->draw_cursor, 
                      V2(ui->current_window->size.x - 2*PADDING,
                         (f32)ui->font.font_size),
                      title);
    
    b32 active = ui->sub_menus[ui->sub_menu_index].is_active;
    
    Assert(ui->sub_menu_index < ArrayCount(ui->sub_menus));
    ++ui->sub_menu_index;
    
    return active;
}


