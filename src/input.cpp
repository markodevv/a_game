#define SHIFT_MODIF 0x1
#define CONTROL_MODIF 0x2

internal b8
ButtonDown(ButtonState button)
{
    return button.is_down;
}

internal b8
ButtonPressed(ButtonState button)
{
    return button.pressed;
}

internal b8
ButtonReleased(ButtonState button)
{
    return button.released;
}

internal b8
ModifierPressed(GameInput* input, u32 modifier)
{
    return (input->modifiers & modifier) == modifier;
}
