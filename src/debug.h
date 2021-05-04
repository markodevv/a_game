#if !defined(DEBUG_H)
#define DEBUG_H

#include "stb_truetype.h"
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

struct DebugMenu
{
    b8 menu_is_active;
};


struct DebugState
{
    MemoryArena arena;

    RenderGroup render_group;
    RenderSetup render_setup;

    HotItem hot_item;
    HotItem next_hot_item;
    InteractingItem interacting_item;
    InteractingItem first_interaction_check;

    SpriteHandle font_sprite_handle;
    stbtt_bakedchar char_metrics[NUM_ASCII];
    f32 font_size;
    f32 x_advance;

    vec2 draw_cursor;
    f32 cursor_start_x;

    char text_fill_buffer[256];
    char text_input_buffer[256];
    u32 text_insert_index;

    f32 game_fps;
    vec2 mouse_pos;
    vec2 prev_mouse_pos;

    vec2 menu_pos;
    vec2 menu_size;
    b8 is_newline;

    u32 current_menu_index;
    DebugMenu menus[32];
};


internal inline void
print_vec3(vec3 v, char* name)
{
    DEBUG_PRINT("v.x: %.2f", v.x);
    DEBUG_PRINT("v.y: %.2f", v.y);
    DEBUG_PRINT("v.z: %.2f", v.z);
}

#endif
