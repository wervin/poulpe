#ifndef POULPE_DISPLAY_H
#define POULPE_DISPLAY_H

#include <stdint.h>

#include "poulpe/error.h"

enum poulpe_error poulpe_display_init(void);
void poulpe_display_draw_chars(ImVec2 position, ImVec4 color, const char *text_begin, const char *text_end);
void poulpe_display_draw_rect_filled(ImVec2 upper_left, ImVec2 lower_right, ImVec4 color);
void poulpe_display_destroy(void);

#endif /* POULPE_DISPLAY_H */