#ifndef DEMO_RENDERER_H
#define DEMO_RENDERER_H

#include <stdint.h>

#include "demo/error.h"

enum demo_error demo_renderer_init(void);

enum demo_error demo_renderer_draw(void);

void demo_renderer_wait_idle(void);

void demo_renderer_destroy(void);

enum demo_error demo_renderer_init_ui(void);

void demo_renderer_refresh_ui(void);

void demo_renderer_destroy_ui(void);

#endif /* DEMO_RENDERER_H */