#ifndef POULPE_DISPLAY_H
#define POULPE_DISPLAY_H

#include <stdint.h>

#include <cglm/cglm.h>

#include "poulpe/error.h"

enum poulpe_error poulpe_display_init(void);
void poulpe_display_draw_chars(vec2 position, vec4 color, const char *text_begin, const char *text_end);
void poulpe_display_draw_rect_filled(vec2 upper_left,
                                     vec2 lower_right,
                                     vec2 upper_left_clipped,
                                     vec2 lower_right_clipped,
                                     vec4 color);
void poulpe_display_destroy(void);

#endif /* POULPE_DISPLAY_H */