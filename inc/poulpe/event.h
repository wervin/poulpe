#ifndef POULPE_EVENT_H
#define POULPE_EVENT_H

#include <stdbool.h>
#include <stdint.h>

#include <cimgui.h>

enum poulpe_event_type
{
    POULPE_EVENT_TYPE_MOUSE = 0,
    POULPE_EVENT_TYPE_KEYBOARD = 1
};

struct poulpe_event
{
    enum poulpe_event_type type;
};

struct poulpe_event_mouse
{
    struct poulpe_event base;
    ImVec2 position;
    bool left_clicked;
    bool left_released;
    bool right_clicked;
    bool right_released;
};

struct poulpe_event_keyboard
{
    struct poulpe_event base;
    uint8_t c;
    bool ctrl;
    bool super;
};

#endif /* POULPE_EVENT_H */