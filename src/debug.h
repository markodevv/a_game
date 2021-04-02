#if !defined(DEBUG_H)
#define DEBUG_H


struct UiItemId
{
    void* id;
};

enum InteractableUiItemType
{
    INTERACTABLE_TYPE_CLICK = 1,
    INTERACTABLE_TYPE_DRAG = 2,
};

struct InteractableUiItem
{
    InteractableUiItemType type;
    UiItemId id;
    InteractableUiItem *next_item;
};


struct InteractableUiItemList
{
    InteractableUiItem* head;
    InteractableUiItem* tail;
};

struct DebugState
{
    MemoryArena arena;
    TemporaryArena temporary_arena;

    InteractableUiItemList ui_item_list;
    InteractableUiItem hot_item;
    InteractableUiItem next_hot_item;
    InteractableUiItem interacting_item;

    vec2 draw_cursor;
    vec2 prev_draw_cursor;

    f32 game_fps;
    vec2 mouse_pos;
    vec2 prev_mouse_pos;
    vec2 menu_pos;
};

#endif
