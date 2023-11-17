#include <cimgui.h>

#include "poulpe/io.h"
#include "poulpe/event.h"

enum poulpe_error poulpe_io_handle_keyboard(struct poulpe_component *component)
{
    enum poulpe_error error = POULPE_ERROR_NONE;

    ImGuiIO *io = igGetIO();

    struct poulpe_event_keyboard event = {0};

    event.base.type = POULPE_EVENT_TYPE_KEYBOARD;
    
    if (!io->InputQueueCharacters.Size)
        return POULPE_ERROR_NONE;
    
    for (int32_t i = 0; i < io->InputQueueCharacters.Size; i++)
    {
        event.c = io->InputQueueCharacters.Data[i];
        error = poulpe_component_notify(component, (struct poulpe_event *) &event);
        if (error != POULPE_ERROR_NONE)
            return error;
    }

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_io_handle_mouse(struct poulpe_component *component)
{
    ImGuiIO *io = igGetIO();

    struct poulpe_event_mouse event = {0};

    ImVec2 min;
    igGetCursorScreenPos(&min);
    ImVec2 max;
    igGetContentRegionAvail(&max);
    max.x += min.x;
    max.y += min.y;
    ImRect region = {min, max};
    if (!ImRect_Contains_Vec2(&region, io->MousePos))
        return POULPE_ERROR_NONE;

    event.base.type = POULPE_EVENT_TYPE_MOUSE;

    event.position.x = io->MousePos.x;
    event.position.y = io->MousePos.y;
    event.left_clicked = io->MouseClicked[0];
    event.left_released = io->MouseReleased[0];
    event.right_clicked = io->MouseClicked[1];
    event.right_released = io->MouseReleased[1];
    
    return poulpe_component_notify(component, (struct poulpe_event *) &event);
}
