struct SubMenu
{
b8 is_active;
};
struct UiWindow
{
vec2 position;
vec2 size;
u32 layer;
UiWindow* next;
};
struct StackedWindow
{
UiWindow* window;
vec2 previous_draw_cursor;
};
struct Colorpicker
{
Color color;
f32 hue;
f32 saturation;
f32 brightness;
b8 is_active;
void* key;
Colorpicker* next;
};
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
Colorpicker colorpickers[64];
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
StackedWindow window_stack[32];
u32 num_stacked_windows;
b8 do_pop_window;
vec2 draw_cursor;
u32 top_layer;
b8 is_newline;
SubMenu sub_menus[64];
u32 sub_menu_index;
i32 screen_height;
i32 screen_width;
};
