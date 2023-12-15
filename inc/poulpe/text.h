#ifndef POULPE_TEXT_H
#define POULPE_TEXT_H

#include <stdint.h>

#include <cimgui.h>

#include <sake/string.h>

#include "poulpe/error.h"

typedef sake_string poulpe_line;
typedef poulpe_line* poulpe_text;

poulpe_text poulpe_text_new(void);
void poulpe_text_free(poulpe_text text);
uint32_t poulpe_text_size(poulpe_text text);
poulpe_text poulpe_text_push_back(poulpe_text text, poulpe_line *line);
poulpe_text poulpe_text_insert(poulpe_text text, uint32_t index, poulpe_line *line);
void poulpe_text_pop_back(poulpe_text text);
void poulpe_text_erase(poulpe_text text, uint32_t index);

poulpe_line poulpe_line_new(const char *begin, const char *end);
void poulpe_line_free(poulpe_line line);
uint32_t poulpe_line_raw_size(poulpe_line line);
uint32_t poulpe_line_utf8_size(poulpe_line line);
poulpe_line poulpe_line_push_back(poulpe_line line, const char *begin, const char *end);
poulpe_line poulpe_line_insert(poulpe_line line, uint32_t index, const char *begin, const char *end);
void poulpe_line_pop_back(poulpe_line line);
void poulpe_line_erase(poulpe_line line, uint32_t index);
void poulpe_line_erase_range(poulpe_line line, uint32_t from, uint32_t to);
uint32_t poulpe_line_utf8_index(poulpe_line line, uint32_t raw_index);
uint32_t poulpe_line_raw_index(poulpe_line line, uint32_t utf8_index);

#endif /* POULPE_TEXT_H */