typedef struct DebugColapsableItem DebugColapsableItem;
struct DebugColapsableItem
{
b8 menu_is_active;
};

typedef struct UiWindow UiWindow;
struct UiWindow
{
vec2 position;
vec2 size;
u32 layer;
};

typedef struct DebugState DebugState;
struct DebugState
{
MemoryArena arena;
TemporaryArena temp_arena;
RenderGroup* render_group;
void* hot_item;
void* next_hot_item;
void* active_item;
GameInput input;
Font font;
char text_input_buffer[256];
u32 text_insert_index;
u8 editbox_value_to_set_size;
b8 editbox_value_is_int;
void* editbox_value_to_set;
f32 game_fps;
vec2 mouse_pos;
vec2 prev_mouse_pos;
UiWindow window_table[64];
UiWindow* current_window;
UiWindow* focused_window;
vec2 draw_cursor;
vec2 last_draw_cursor;
u32 top_layer;
b8 is_newline;
DebugColapsableItem window_items[512];
u32 current_item_index;
i32 screen_height;
i32 screen_width;
};

