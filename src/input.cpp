#define SHIFT_MODIF 0x1
#define CONTROL_MODIF 0x2

internal b8
button_down(ButtonState button)
{
    return button.is_down;
}

internal b8
button_repeat(ButtonState button)
{
    return (button.repeat_count > 0);
}

internal b8
button_pressed(ButtonState button)
{
    return button.pressed;
}

internal b8
button_released(ButtonState button)
{
    return button.released;
}

internal b8
modifier_pressed(GameInput* input, u32 modifier)
{
    return (input->modifiers & modifier) == modifier;
}
