#include <cimgui.h>

#include "poulpe/io.h"
#include "poulpe/event.h"

static bool _update_dirty_bit(bool status, struct poulpe_event *event);

enum poulpe_error poulpe_io_handle_keyboard(struct poulpe_component *component)
{
    enum poulpe_error error = POULPE_ERROR_NONE;
    
    ImGuiIO *io = igGetIO();

    struct poulpe_event_keyboard event = {0};

    event.base.type = POULPE_EVENT_TYPE_KEYBOARD;

    event.delete = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_Delete, true), (struct poulpe_event *) &event);
    event.backspace = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_Backspace, true), (struct poulpe_event *) &event);
    event.right = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_RightArrow, true), (struct poulpe_event *) &event);
    event.left = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_LeftArrow, true), (struct poulpe_event *) &event);
    event.up = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_UpArrow, true), (struct poulpe_event *) &event);
    event.down = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_DownArrow, true), (struct poulpe_event *) &event);
    event.enter = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_Enter, true), (struct poulpe_event *) &event);
    event.tab = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_Tab, true), (struct poulpe_event *) &event);
    event.page_down = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_PageDown, true), (struct poulpe_event *) &event);
    event.page_up = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_PageUp, true), (struct poulpe_event *) &event);
    event.escape = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_Escape, true), (struct poulpe_event *) &event);
    event.left_ctrl = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_LeftCtrl, true), (struct poulpe_event *) &event);
    event.left_shift = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_LeftShift, true), (struct poulpe_event *) &event);
    event.left_alt = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_LeftAlt, true), (struct poulpe_event *) &event);
    event.a = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_A, true), (struct poulpe_event *) &event);
    event.x = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_X, true), (struct poulpe_event *) &event);
    event.c = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_C, true), (struct poulpe_event *) &event);
    event.v = _update_dirty_bit(igIsKeyPressed_Bool(ImGuiKey_V, true), (struct poulpe_event *) &event);

    event.data = io->InputQueueCharacters.Data;
    event.count = io->InputQueueCharacters.Size;

    if (!event.count && !event.base.dirty)
        return POULPE_ERROR_NONE;

    error = poulpe_component_notify(component, (struct poulpe_event *) &event);
    if (error != POULPE_ERROR_NONE)
        return error;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_io_handle_mouse(struct poulpe_component *component)
{
    ImGuiIO *io = igGetIO();
    ImGuiWindow *window = igGetCurrentWindowRead();

    struct poulpe_event_mouse event = {0};

    ImVec2 min;
    igGetCursorScreenPos(&min);
    ImVec2 max;
    igGetContentRegionAvail(&max);
    min.x += window->Scroll.x;
    min.y += window->Scroll.y;
    max.x += min.x;
    max.y += min.y;
    ImRect region = {min, max};
    if (!ImRect_Contains_Vec2(&region, io->MousePos))
        return POULPE_ERROR_NONE;

    event.base.type = POULPE_EVENT_TYPE_MOUSE;

    event.left_clicked = _update_dirty_bit(io->MouseClicked[0], (struct poulpe_event *) &event);
    event.left_released = _update_dirty_bit(io->MouseReleased[0], (struct poulpe_event *) &event);
    event.right_clicked = _update_dirty_bit(io->MouseClicked[1], (struct poulpe_event *) &event);
    event.right_released = _update_dirty_bit(io->MouseReleased[1], (struct poulpe_event *) &event);
    event.dragged = _update_dirty_bit(igIsMouseDragging(0, -1.0f), (struct poulpe_event *) &event);

    event.position.x = io->MousePos.x;
    event.position.y = io->MousePos.y;
    
    return poulpe_component_notify(component, (struct poulpe_event *) &event);
}

static bool _update_dirty_bit(bool status, struct poulpe_event *event)
{
    event->dirty = event->dirty || status;
    return status;
}