#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include <cimgui.h>

#include "poulpe/text.h"
#include "poulpe/log.h"

#include <sake/vector.h>
#include <sake/string.h>

static sake_string _line_to_str(poulpe_line line);
static sake_string _subset_line_to_str(poulpe_line line, uint32_t start, uint32_t end);

poulpe_text poulpe_text_new(void)
{
    return sake_vector_new(sizeof(poulpe_line), sake_vector_free);
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

poulpe_line poulpe_line_new(void)
{
    return sake_vector_new(sizeof(struct poulpe_glyph), NULL);
}

void poulpe_line_free(poulpe_line line)
{
    sake_vector_free(line);
}

uint32_t poulpe_line_size(poulpe_line line)
{
    return sake_vector_size(line);
}

float poulpe_line_full_textsize(poulpe_line line)
{
    ImVec2 line_size;
    sake_string line_str = _line_to_str(line);
    ImFont_CalcTextSizeA(&line_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line_str, NULL, NULL);
    sake_string_free(line_str);
    return line_size.x;
}

float poulpe_line_subset_textsize(poulpe_line line, uint32_t start, uint32_t end)
{
    ImVec2 line_size;
    sake_string line_str = _subset_line_to_str(line, start, end);
    ImFont_CalcTextSizeA(&line_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, line_str, NULL, NULL);
    sake_string_free(line_str);
    return line_size.x;
}

poulpe_line poulpe_line_push_back(poulpe_line line, struct poulpe_glyph *glyph)
{
    return sake_vector_push_back(line, glyph);
}

poulpe_line poulpe_line_insert(poulpe_line line, uint32_t index, struct poulpe_glyph *glyph)
{
    return sake_vector_insert(line, index, glyph);
}

void poulpe_line_pop_back(poulpe_line line)
{
    sake_vector_pop_back(line);
}

void poulpe_line_erase(poulpe_line line, uint32_t index)
{
                                                                                                                                    sake_vector_erase(line, index);
}

poulpe_line poulpe_line_copy(poulpe_line from, poulpe_line to)
{
    return sake_vector_copy(from, to);
}

enum poulpe_error poulpe_glyph_from_uf8(struct poulpe_glyph *glyph, uint8_t *buffer)
{
    if (buffer[0] < 0x80)
    {
        glyph->utf8[0] = buffer[0];
        glyph->size = 1;
        return POULPE_ERROR_NONE;
    }

    if ((buffer[0] & 0xE0) == 0xC0)
    {
        glyph->utf8[0] = buffer[0];
        glyph->utf8[1] = buffer[1];
        glyph->size = 2;
        return POULPE_ERROR_NONE;
    }

    POULPE_LOG_ERROR(POULPE_ERROR_NOT_IMPLEMENTED, "Up to 2 bytes is supported");
    return POULPE_ERROR_NOT_IMPLEMENTED;
}

enum poulpe_error poulpe_glyph_from_char(struct poulpe_glyph *glyph, uint32_t c)
{
    if (c < 0x80)
    {
        glyph->utf8[0] = (uint8_t)c;
        glyph->size = 1;
        return POULPE_ERROR_NONE;
    }

    if (c < 0x800)
    {
        glyph->utf8[0] = (uint8_t)(0xc0 + (c >> 6));
        glyph->utf8[1] = (uint8_t)(0x80 + (c & 0x3f));
        glyph->size = 2;
        return POULPE_ERROR_NONE;
    }

    POULPE_LOG_ERROR(POULPE_ERROR_NOT_IMPLEMENTED, "Up to 2 bytes is supported");
    return POULPE_ERROR_NOT_IMPLEMENTED;
}

static sake_string _line_to_str(poulpe_line line)
{
    uint32_t length = sake_vector_size(line);
    /* worst case */
    char buffer[2 * length + 1];

    uint32_t j = 0;
    for (uint32_t i = 0; i < length; i++)
    {
        memcpy(buffer + j, line[i].utf8, line[i].size);
        j += line[i].size;
    }

    buffer[j] = '\0';
    return sake_string_new(buffer);
}

static sake_string _subset_line_to_str(poulpe_line line, uint32_t start, uint32_t end)
{
    /* worst case */
    char buffer[2 * (end - start) + 1];

    uint32_t j = 0;
    for (uint32_t i = start; i < end; i++)
    {
        memcpy(buffer + j, line[i].utf8, line[i].size);
        j += line[i].size;
    }

    buffer[j] = '\0';
    return sake_string_new(buffer);
}