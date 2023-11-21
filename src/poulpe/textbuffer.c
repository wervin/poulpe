#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "poulpe/textbuffer.h"
#include "poulpe/log.h"

struct poulpe_textbuffer * poulpe_textbuffer_new(void)
{
    struct poulpe_textbuffer *textbuffer;

    textbuffer = calloc(1, sizeof(struct poulpe_textbuffer));
    if (!textbuffer)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate textbuffer");
        return NULL;
    }
    return textbuffer;
}

enum poulpe_error poulpe_textbuffer_open(struct poulpe_textbuffer * textbuffer, const char *filename)
{
    uint32_t size;
    uint8_t *buffer;

    FILE *fd = fopen(filename, "r");
    if (!fd)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, filename);
        return POULPE_ERROR_MEMORY;
    }

    fseek(fd, 0, SEEK_END);
    size = ftell(fd);

    buffer = malloc(size * sizeof(uint8_t));
    if (buffer == NULL)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate memory");
        return POULPE_ERROR_MEMORY;
    }

    rewind(fd);
    fread(buffer, 1, size, fd);
    fclose(fd);

    textbuffer->text = poulpe_text_new();
    if (!textbuffer->text)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate text");
        return POULPE_ERROR_MEMORY;
    }

    poulpe_line line = poulpe_line_new();
    if (!line)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate line");
        return POULPE_ERROR_MEMORY;
    }

    uint32_t i = 0;
    while(i < size)
    {
        if (buffer[i] == '\r')
        {
            i++;
            continue;
        }
        
        if (buffer[i] == '\n')
        {
            textbuffer->text = poulpe_text_push_back(textbuffer->text, &line);
            if (!textbuffer->text)
            {
                POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back line");
                return POULPE_ERROR_MEMORY;
            }

            line = poulpe_line_new();
            if (!line)
            {
                POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate line");
                return POULPE_ERROR_MEMORY;
            }
            
            i++;
            continue;
        }

        struct poulpe_glyph g = {0};
        enum poulpe_error error = poulpe_glyph_from_uf8(&g, buffer + i);
        if (error != POULPE_ERROR_NONE)
            return error;

        line = poulpe_line_push_back(line, &g);
        if (!line)
        {
            POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back glyph");
            return POULPE_ERROR_MEMORY;
        }

        i += g.size;
    }

    if (poulpe_line_size(line))
    {
        textbuffer->text = poulpe_text_push_back(textbuffer->text, &line);
        if (!textbuffer->text)
        {
            POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back line");
            return POULPE_ERROR_MEMORY;
        }
    }

    free(buffer);
    return POULPE_ERROR_NONE;
}

void poulpe_textbuffer_free(struct poulpe_textbuffer * textbuffer)
{
    poulpe_text_free(textbuffer->text);
    free(textbuffer);
}

uint32_t poulpe_textbuffer_text_size(struct poulpe_textbuffer * textbuffer)
{
    return poulpe_text_size(textbuffer->text);
}

enum poulpe_error poulpe_textbuffer_new_line(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t glyph_index)
{
    poulpe_line line = poulpe_line_new();
    if (!line)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate line");
        return POULPE_ERROR_MEMORY;
    }

    line = poulpe_line_copy(textbuffer->text[line_index], line);

    for (uint32_t i = 0; i < glyph_index; i++)
        poulpe_line_erase(textbuffer->text[line_index], 0);

    for (uint32_t i = poulpe_line_size(line); i > glyph_index; i--)
        poulpe_line_pop_back(line);

    enum poulpe_error error =  poulpe_textbuffer_text_insert(textbuffer, line_index, &line);
    if (error != POULPE_ERROR_NONE)
        return error;
    
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textbuffer_text_push_back(struct poulpe_textbuffer * textbuffer, poulpe_line *line)
{
    textbuffer->text = poulpe_text_push_back(textbuffer->text, line);
    if (!textbuffer->text)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back line");
        return POULPE_ERROR_MEMORY;
    }
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textbuffer_text_insert(struct poulpe_textbuffer * textbuffer, uint32_t line_index, poulpe_line *line)
{
    textbuffer->text = poulpe_text_insert(textbuffer->text, line_index, line);
    if (!textbuffer->text)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to insert line");
        return POULPE_ERROR_MEMORY;
    }
    return POULPE_ERROR_NONE;
}

void poulpe_textbuffer_text_erase(struct poulpe_textbuffer * textbuffer, uint32_t line_index)
{
    poulpe_text_erase(textbuffer->text, line_index);
}

uint32_t poulpe_textbuffer_line_size(struct poulpe_textbuffer * textbuffer, uint32_t line_index)
{
    return poulpe_line_size(textbuffer->text[line_index]);
}

float poulpe_textbuffer_line_full_textsize(struct poulpe_textbuffer * textbuffer, uint32_t line_index)
{
    return poulpe_line_full_textsize(textbuffer->text[line_index]);
}

float poulpe_textbuffer_line_subset_textsize(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t start, uint32_t end)
{
    return poulpe_line_subset_textsize(textbuffer->text[line_index], start, end);
}

enum poulpe_error poulpe_textbuffer_line_push_back(struct poulpe_textbuffer * textbuffer, uint32_t line_index, struct poulpe_glyph *glyph)
{
    textbuffer->text[line_index] = poulpe_line_push_back(textbuffer->text[line_index], glyph);
    if (!textbuffer->text[line_index])
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back glyph");
        return POULPE_ERROR_MEMORY;
    }
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textbuffer_line_insert(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t glyph_index, struct poulpe_glyph *glyph)
{
    textbuffer->text[line_index] = poulpe_line_insert(textbuffer->text[line_index], glyph_index, glyph);
    if (!textbuffer->text[line_index])
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to insert glyph");
        return POULPE_ERROR_MEMORY;
    }
    return POULPE_ERROR_NONE;
}

void poulpe_textbuffer_line_erase(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t glyph_index)
{
    poulpe_line_erase(textbuffer->text[line_index], glyph_index);
}

struct poulpe_glyph * poulpe_textbuffer_line_at(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t glyph_index)
{
    return &textbuffer->text[line_index][glyph_index];
}