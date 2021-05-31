#if !defined(INPUT_H)
#define INPUT_H

struct ButtonState
{
    b8 is_down;
    b8 released;
    b8 pressed;

    u8 repeat_count;
};

struct MouseInput
{
    vec2 position;
    i32 wheel_delta;
    b8 moved;
};

#define SHIFT_MODIF 0x1
#define CONTROL_MODIF 0x2

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
        };
        ButtonState buttons[13];
    };
    u32 modifiers;
    MouseInput mouse;
    char character;
};

internal inline b8
button_down(ButtonState button)
{
    return button.is_down;
}

internal inline b8
button_repeat(ButtonState button)
{
    return (button.repeat_count > 0);
}

internal inline b8
button_pressed(ButtonState button)
{
    return button.pressed;
}

internal inline b8
button_released(ButtonState button)
{
    return button.released;
}

internal inline b8
modifier_pressed(GameInput* input, u32 modifier)
{
    return (input->modifiers & modifier) == modifier;
}

#endif
