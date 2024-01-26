#ifndef POULPE_EDITOR_H
#define POULPE_EDITOR_H

#include "poulpe/error.h"
#include "poulpe/language.h"

struct poulpe_textview;
struct poulpe_statusbar;

struct poulpe_editor
{    
    struct poulpe_textview *textview;
    struct poulpe_statusbar *statusbar;
};

struct poulpe_editor* poulpe_editor_new(const char *path);
void poulpe_editor_free(struct poulpe_editor *editor);
enum poulpe_error poulpe_editor_draw(struct poulpe_editor *editor);
const char * poulpe_editor_filename(struct poulpe_editor *editor);
enum poulpe_error poulpe_editor_set_language(struct poulpe_editor *editor, enum poulpe_language_type language);

#endif /* POULPE_EDITOR_H */