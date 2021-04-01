#if !defined(DEBUG_H)
#define DEBUG_H


struct UiItemId
{
    void* id;
};

enum InteractableUiItemType
{
    INTERACTABLE_UI_ITEM_TYPE_CLICK,
    INTERACTABLE_UI_ITEM_TYPE_DRAG,
};

struct InteractableUiItem
{
    InteractableUiItemType type;
    vec2 pos;
    vec2 size;
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
    InteractableUiItem active_item;

    f32 game_fps;
    vec2 mouse_position;
};

#endif
