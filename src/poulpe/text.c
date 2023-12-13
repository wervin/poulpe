#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include <cimgui.h>

#include "poulpe/text.h"
#include "poulpe/log.h"

#include <sake/vector.h>
#include <sake/string.h>

poulpe_text poulpe_text_new(void)
{
    return sake_vector_new(sizeof(poulpe_line), (void (*)(void *))poulpe_line_free);
}

void poulpe_text_free(poulpe_text text)
{
    sake_vector_free(text);
}

uint32_t poulpe_text_size(poulpe_text text)
{
    return sake_vector_size(text);
}

poulpe_text poulpe_text_push_back(poulpe_text text, poulpe_line *line)
{
    return sake_vector_push_back(text, line);
}

poulpe_text poulpe_text_insert(poulpe_text text, uint32_t index, poulpe_line *line)
{
    return sake_vector_insert(text, index, line);
}

void poulpe_text_pop_back(poulpe_text text)
{
    sake_vector_pop_back(text);
}

void poulpe_text_erase(poulpe_text text, uint32_t index)
{
    sake_vector_erase(text, index);
}

poulpe_line poulpe_line_new(const char *string)
{
    return sake_string_new(string);
}

poulpe_line poulpe_line_new_range(const char *begin, const char *end)
{
    return sake_string_new_range(begin, end);
}

void poulpe_line_free(poulpe_line line)
{
    sake_string_free(line);
}

uint32_t poulpe_line_raw_size(poulpe_line line)
{
    return sake_string_raw_size(line);
}

uint32_t poulpe_line_utf8_size(poulpe_line line)
{
    return sake_string_utf8_size(line);
}

poulpe_line poulpe_line_push_back(poulpe_line line, const char *data)
{
    return sake_string_push_back(line, data);
}

poulpe_line poulpe_line_insert(poulpe_line line, uint32_t index, const char *data)
{
    return sake_string_insert(line, index, data);
}

void poulpe_line_pop_back(poulpe_line line)
{
    sake_string_pop_back(line);
}

void poulpe_line_erase(poulpe_line line, uint32_t index)
{
    sake_string_erase(line, index);
}

void poulpe_line_erase_range(poulpe_line line, uint32_t from, uint32_t to)
{
    sake_string_erase_range(line, from, to);
}

uint32_t poulpe_line_utf8_index(poulpe_line line, uint32_t raw_index)
{
    return sake_string_utf8_index(line, raw_index);
}

uint32_t poulpe_line_raw_index(poulpe_line line, uint32_t utf8_index)
{
    return sake_string_raw_index(line, utf8_index);
}