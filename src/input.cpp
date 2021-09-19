#define SHIFT_MODIF 0x1
#define CONTROL_MODIF 0x2

internal b32
ButtonDown(GameInput* input, ButtonCode button)
{
    return input->buttons[button].is_down;
}

internal b32
ButtonPressed(GameInput* input, ButtonCode button)
{
    return input->buttons[button].pressed;
}

internal b32
ButtonReleased(GameInput* input, ButtonCode button)
{
    return input->buttons[button].released;
}

internal b32
ModifierPressed(GameInput* input, u32 modifier)
{
    return (input->modifiers & modifier) == modifier;
}
