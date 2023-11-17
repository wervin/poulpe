#include <stdlib.h>
#include <stdio.h>

#include "poulpe/text.h"

#include "sake/vector.h"

poulpe_text poulpe_text_new(void)
{
    poulpe_text text = sake_vector_new(sizeof(poulpe_line), sake_vector_free);
    if (!text)
        return NULL;
    return text;
}

void poulpe_text_free(poulpe_text text)
{
    sake_vector_free(text);
}

poulpe_text poulpe_text_push_back(poulpe_text text, poulpe_line *line)
{
    text = sake_vector_push_back(text, line);
    if (!text)
        return NULL;
    return text;
}

void poulpe_text_print(poulpe_text text)
{
    for (uint32_t i = 0; i < sake_vector_size(text); i++)
    {
        poulpe_line line = text[i];
        for (uint32_t j = 0; j < sake_vector_size(line); j++)
        {
            printf("%c", line[j].c);
        }
        printf("\n");
    }
}

poulpe_line poulpe_line_new(void)
{
    poulpe_line line = sake_vector_new(sizeof(struct poulpe_glyph), NULL);
    if (!line)
        return NULL;
    return line;
}

void poulpe_line_free(poulpe_line line)
{
    sake_vector_free(line);
}

poulpe_line poulpe_line_push_back(poulpe_line line, struct poulpe_glyph *glyph)
{
    line = sake_vector_push_back(line, glyph);
    if (!line)
        return NULL;
    return line;
}

poulpe_line poulpe_line_insert(poulpe_line line, uint32_t index, struct poulpe_glyph *glyph)
{
    line = sake_vector_insert(line, index, glyph);
    if (!line)
        return NULL;
    return line;
}

sake_string poulple_line_to_str(poulpe_line line)
{
    uint32_t length = sake_vector_size(line);
    char buffer[length + 1];

    for (uint32_t i = 0; i < length; i++)
        buffer[i] = line[i].c;

    buffer[length] = '\0';
    return sake_string_new(buffer);
}