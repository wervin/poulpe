#ifndef POULPE_EDITOR_H
#define POULPE_EDITOR_H

#include <stdint.h>

#include "poulpe/error.h"

struct poulpe_textview;
struct poulpe_statusbar;

struct poulpe_editor
{    
    struct poulpe_textview *textview;
    struct poulpe_statusbar *statusbar;

    ImFont *icon_font;
    ImFont *large_font;
    ImFont *small_font;
};

struct poulpe_editor* poulpe_editor_new(const char *path);
void poulpe_editor_free(struct poulpe_editor *editor);
enum poulpe_error poulpe_editor_draw(struct poulpe_editor *editor);
const char * poulpe_editor_filename(struct poulpe_editor *editor);
const ImVec2 * poulpe_editor_cursor_position(struct poulpe_editor *editor);

#endif /* POULPE_EDITOR_H */