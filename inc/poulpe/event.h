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
    bool dirty;
};

struct poulpe_event_mouse
{
    struct poulpe_event base;
    ImVec2 position;
    ImVec2 left_clicked_position;
    bool left_clicked;
    bool left_released;
    ImVec2 right_clicked_position;
    bool right_clicked;
    bool right_released;
    bool dragged;
};

struct poulpe_event_keyboard
{
    struct poulpe_event base;
    uint16_t *data;
    uint32_t count;
    bool delete;
    bool backspace;
    bool right;
    bool left;
    bool up;
    bool down;
    bool enter;
    bool tab;
    bool page_down;
    bool page_up;
    bool escape;
    bool ctrl;
    bool shift;
    bool alt;
    bool a;
    bool c;
    bool v;
    bool x;
    bool z;
    bool y;
};

#endif /* POULPE_EVENT_H */