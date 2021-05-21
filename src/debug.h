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

struct DebugMenuItem
{
    b8 menu_is_active;
};

struct DebugMenu
{
    vec2 position;
    vec2 size;
};



struct DebugState
{
    MemoryArena arena;
    TemporaryArena temp_arena;

    RenderGroup* render_group;

    HotItem hot_item;
    HotItem next_hot_item;
    InteractingItem interacting_item;
    InteractingItem first_interaction_check;

    Font font;

    vec2 draw_cursor;
    f32 cursor_start_x;

    char text_input_buffer[256];
    u32 text_insert_index;

    f32 game_fps;
    vec2 mouse_pos;
    vec2 prev_mouse_pos;

    DebugMenu menu_table[512];
    DebugMenu* menu;
    u32 menu_index;

    b8 is_newline;

    u32 current_menu_index;
    DebugMenuItem menus[32];
};


internal inline void
print_vec3(vec3 v, char* name)
{
    PRINT("v.x: %.2f", v.x);
    PRINT("v.y: %.2f", v.y);
    PRINT("v.z: %.2f", v.z);
}
internal DebugMenu*
get_menu(DebugState* debug, char* key)
{
    u64 hash = 5381;
    i32 c;
    
    while((c = *key++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    u16 hash_index = hash % ArrayCount(debug->menu_table);

    DebugMenu* debug_menu = debug->menu_table + hash_index;

    return debug_menu;
}


#endif
