#pragma once

typedef struct SubMenu SubMenu;
struct SubMenu
{
    b8 is_active;
};
// @Print
typedef struct UiWindow UiWindow;
struct UiWindow
{
    vec2 position;
    vec2 size;
    u32 layer;

    char *id;


    // @NoPrint
    UiWindow *next;
};
typedef struct WindowSerializeData WindowSerializeData;
struct WindowSerializeData
{
    u32 name_size;
    char name[64];
    vec2 position;
    vec2 size;
};
typedef struct UiFileHeader UiFileHeader;
struct UiFileHeader
{
    u32 window_count;
};
typedef struct StackedWindow StackedWindow;
struct StackedWindow
{
    UiWindow *window;
    vec2 previous_draw_cursor;
};
typedef struct Colorpicker Colorpicker;
struct Colorpicker
{
    Color color;

    f32 hue;
    f32 saturation;
    f32 brightness;
    b8 is_active;

    void *key;
    Colorpicker *next;
};
typedef struct ProfileEntry ProfileEntry;
struct ProfileEntry
{
    char *name;
    f64 elapsed_sec;

    ProfileEntry *next;
};
typedef struct DebugState DebugState;
struct DebugState
{
    MemoryArena arena;
    MemoryArena temp_arena;

    TemporaryArena temp_memory;

    RenderGroup *render_group;

    ProfileEntry *profile_entries;
    u64 frame_start_cycles;

    void *hot_item;
    void *next_hot_item;
    void *active_item;

    GameInput input;

    Font font;

    Colorpicker colorpickers[64];

    char text_input_buffer[256];
    u32 text_insert_index;

    u8 editbox_value_to_set_size;
    b8 editbox_value_is_int;
    void *editbox_value_to_set;

    f32 game_fps;
    vec2 mouse_pos;
    vec2 prev_mouse_pos;

    UiWindow *window_table[64];
    UiWindow *current_window;
    UiWindow *focused_window;

    StackedWindow window_stack[32];
    u32 num_stacked_windows;

    vec2 draw_cursor;
    u32 top_layer;
    b8 is_newline;

    SubMenu sub_menus[64];
    u32 sub_menu_index;

    i32 screen_height;
    i32 screen_width;

    b8 have_read_config;
};
