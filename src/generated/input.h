typedef struct ButtonState ButtonState;
struct ButtonState
{
    b8 is_down;
    b8 released;
    b8 pressed;
};
typedef struct MouseInput MouseInput;
struct MouseInput
{
    vec2 position;
    i32 wheel_delta;
};
typedef enum ButtonCode
{
    BUTTON_LEFT,
















    BUTTON_RIGHT,

















    BUTTON_UP,


















    BUTTON_DOWN,



















    BUTTON_FORWARD,




















    BUTTON_BACK,





















    BUTTON_PAUSE,






















    BUTTON_MOUSE_LEFT,























    BUTTON_MOUSE_RIGHT,
























    BUTTON_BACKSPACE,

























    BUTTON_ESCAPE,


























    BUTTON_ENTER,



























    BUTTON_F1,




























    NUM_BUTTONS,
}
ButtonCode;
typedef struct GameInput GameInput;
struct GameInput
{
    ButtonState buttons[NUM_BUTTONS];
    u32 modifiers;
    MouseInput mouse;
    char character;
};
