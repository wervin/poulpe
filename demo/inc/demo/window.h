#ifndef DEMO_WINDOW_H
#define DEMO_WINDOW_H

#include <stdint.h>
#include <stdbool.h>

#include "demo/error.h"

struct demo_window_info
{
    uint32_t default_width;
    uint32_t default_height;
    bool fullscreen;
    const char *application_name;
};

enum demo_error demo_window_init(struct demo_window_info *info);

void demo_window_show(void);

void demo_window_get_extent(int *width, int *height);

bool demo_window_should_close(void);

void demo_window_poll_events(void);

void demo_window_wait_events(void);

enum demo_error demo_window_create_surface(void *instance, void *surface);

const char** demo_window_get_required_extensions(uint32_t* count);

void demo_window_destroy(void);

enum demo_error primsa_window_init_ui(void);

void demo_window_refresh_ui(void);

void demo_window_destroy_ui(void);

#endif /* DEMO_WINDOW_H */