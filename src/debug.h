#if !defined(DEBUG_H)
#define DEBUG_H

#include "renderer.h"


enum InterationType
{
    INTERACTION_TYPE_CLICK = 1,
    INTERACTION_TYPE_DRAG = 2,
    INTERACTION_TYPE_EDIT = 3,
};

enum UiItemType
{
    UI_ITEM_EDITBOX = 1,
};

struct InteractingItem
{
    void* id;
    InterationType type;
};

struct HotItem
{
    void* id;
    UiItemType ui_item_type;
};

struct DebugColapsableItem
{
    b8 menu_is_active;
};

struct DebugWindow
{
    vec2 position;
    vec2 size;

    u32 layer;
};


struct DebugState
{
    MemoryArena arena;
    TemporaryArena temp_arena;

    RenderGroup* render_group;

    HotItem hot_item;
    HotItem next_hot_item;
    InteractingItem interacting_item;
    b8 is_first_interaction;

    Font font;

    vec2 draw_cursor;
    f32 cursor_start_x;

    char text_input_buffer[256];
    u32 text_insert_index;

    f32 game_fps;
    vec2 mouse_pos;
    vec2 prev_mouse_pos;

    DebugWindow window_table[512];
    DebugWindow* window;
    DebugWindow* focused_window;
    u32 current_top_layer;


    b8 is_newline;

    DebugColapsableItem window_items[512];
    u32 current_item_index;
};



#endif
