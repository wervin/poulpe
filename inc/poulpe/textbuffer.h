#ifndef POULPE_TEXTBUFFER_H
#define POULPE_TEXTBUFFER_H

#include <stdbool.h>
#include <stdint.h>

#include <sake/string.h>

#include <tree_sitter/api.h>

#include "poulpe/text.h"
#include "poulpe/error.h"

struct poulpe_textbuffer
{
    poulpe_text text;
    sake_string filename;
    sake_string path;
    bool dirty;
};

struct poulpe_textbuffer * poulpe_textbuffer_new(void);
void poulpe_textbuffer_free(struct poulpe_textbuffer * textbuffer);
enum poulpe_error poulpe_textbuffer_open_file(struct poulpe_textbuffer * textbuffer, const char *path);

#endif /* POULPE_TEXTBUFFER_H */