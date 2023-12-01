#ifndef POULPE_TEXTBUFFER_H
#define POULPE_TEXTBUFFER_H

#include <stdbool.h>
#include <stdint.h>

#include <sake/string.h>

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

uint32_t poulpe_textbuffer_text_size(struct poulpe_textbuffer * textbuffer);
enum poulpe_error poulpe_textbuffer_new_line(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t glyph_index);
enum poulpe_error poulpe_textbuffer_text_push_back(struct poulpe_textbuffer * textbuffer, poulpe_line *line);
enum poulpe_error poulpe_textbuffer_text_insert(struct poulpe_textbuffer * textbuffer, uint32_t line_index, poulpe_line *line);
void poulpe_textbuffer_text_erase(struct poulpe_textbuffer * textbuffer, uint32_t line_index);

uint32_t poulpe_textbuffer_line_size(struct poulpe_textbuffer * textbuffer, uint32_t line_index);
float poulpe_textbuffer_line_full_textsize(struct poulpe_textbuffer * textbuffer, uint32_t line_index);
float poulpe_textbuffer_line_subset_textsize(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t start, uint32_t end);
enum poulpe_error poulpe_textbuffer_line_push_back(struct poulpe_textbuffer * textbuffer, uint32_t line_index, struct poulpe_glyph *glyph);
enum poulpe_error poulpe_textbuffer_line_insert(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t glyph_index, struct poulpe_glyph *glyph);
void poulpe_textbuffer_line_erase(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t glyph_index);

struct poulpe_glyph * poulpe_textbuffer_line_at(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t glyph_index);

#endif /* POULPE_TEXTBUFFER_H */