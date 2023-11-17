#ifndef POULPE_TEXT_H
#define POULPE_TEXT_H

#include <stdint.h>

#include "poulpe/error.h"

#include <sake/string.h>

struct poulpe_glyph
{
    uint8_t c;
};

typedef struct poulpe_glyph* poulpe_line;
typedef poulpe_line* poulpe_text;

poulpe_text poulpe_text_new(void);
void poulpe_text_free(poulpe_text text);
poulpe_text poulpe_text_push_back(poulpe_text text, poulpe_line *line);
void poulpe_text_print(poulpe_text text);

poulpe_line poulpe_line_new(void);
void poulpe_line_free(poulpe_line line);
poulpe_line poulpe_line_push_back(poulpe_line line, struct poulpe_glyph *glyph);
poulpe_line poulpe_line_insert(poulpe_line line, uint32_t index, struct poulpe_glyph *glyph);
sake_string poulple_line_to_str(poulpe_line line);

#endif /* POULPE_TEXT_H */