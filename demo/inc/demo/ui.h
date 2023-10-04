#ifndef DEMO_UI_H
#define DEMO_UI_H

#include <stdint.h>

#include "demo/error.h"

enum demo_error demo_ui_init(void);

void demo_ui_draw(void);

void demo_ui_destroy(void);

#endif /* DEMO_UI_H */