struct ButtonState
{
b8 is_down;
b8 released;
b8 pressed;
};
struct MouseInput
{
vec2 position;
i32 wheel_delta;
};
struct GameInput
{
union 
{
struct 
{
ButtonState move_left;
ButtonState move_right;
ButtonState move_up;
ButtonState move_down;
ButtonState move_forward;
ButtonState move_back;
ButtonState pause_button;
ButtonState left_mouse_button;
ButtonState right_mouse_button;
ButtonState backspace;
ButtonState escape;
ButtonState enter;
ButtonState f1;
};
    ButtonState buttons[13];
};
u32 modifiers;
MouseInput mouse;
char character;
};
