#ifndef POULPE_TEXT_H
#define POULPE_TEXT_H

#include <stdint.h>

#include "poulpe/error.h"

struct poulpe_glyph
{
    uint8_t utf8[2];
    uint32_t size;
};

typedef struct poulpe_glyph* poulpe_line;
typedef poulpe_line* poulpe_text;

poulpe_text poulpe_text_new(void);
void poulpe_text_free(poulpe_text text);
uint32_t poulpe_text_size(poulpe_text text);
poulpe_text poulpe_text_push_back(poulpe_text text, poulpe_line *line);
poulpe_text poulpe_text_insert(poulpe_text text, uint32_t index, poulpe_line *line);
void poulpe_text_pop_back(poulpe_text text);
void poulpe_text_erase(poulpe_text text, uint32_t index);

poulpe_line poulpe_line_new(void);
void poulpe_line_free(poulpe_line line);
uint32_t poulpe_line_size(poulpe_line line);
float poulpe_line_full_textsize(poulpe_line line);
float poulpe_line_subset_textsize(poulpe_line line, uint32_t start, uint32_t end);
poulpe_line poulpe_line_push_back(poulpe_line line, struct poulpe_glyph *glyph);
poulpe_line poulpe_line_insert(poulpe_line line, uint32_t index, struct poulpe_glyph *glyph);
void poulpe_line_pop_back(poulpe_line line);
void poulpe_line_erase(poulpe_line line, uint32_t index);
poulpe_line poulpe_line_copy(poulpe_line from, poulpe_line to);

enum poulpe_error poulpe_glyph_from_uf8(struct poulpe_glyph *glyph, uint8_t *buffer);
enum poulpe_error poulpe_glyph_from_char(struct poulpe_glyph *glyph, uint32_t c);

#endif /* POULPE_TEXT_H */