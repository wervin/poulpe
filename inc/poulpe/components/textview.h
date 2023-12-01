#ifndef POULPE_COMPONENTS_TEXTVIEW_H
#define POULPE_COMPONENTS_TEXTVIEW_H

#include "poulpe/component.h"

#include "poulpe/error.h"

struct poulpe_linenumber;
struct poulpe_textedit;
struct poulpe_textbuffer;

struct poulpe_textview
{
    struct poulpe_component base;
    struct poulpe_textbuffer *textbuffer;
    struct poulpe_textedit *textedit;
    struct poulpe_linenumber *linenumber;
};

enum poulpe_error poulpe_textview_open_file(struct poulpe_textview *textview, const char *path);

#endif /* POULPE_COMPONENTS_TEXTVIEW_H */