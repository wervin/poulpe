#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include <sake/utils.h>
#include <sake/macro.h>

#include <tree_sitter/api.h>

#include "poulpe/textbuffer.h"
#include "poulpe/log.h"
#include "poulpe/theme.h"
#include "poulpe/text.h"
#include "poulpe/language.h"

static const char * _read(void *payload, uint32_t byte_index, TSPoint position, uint32_t *bytes_read);
static enum poulpe_error _init_parser(struct poulpe_textbuffer *textbuffer);

struct poulpe_textbuffer * poulpe_textbuffer_new(void)
{
    struct poulpe_textbuffer *textbuffer;

    textbuffer = calloc(1, sizeof(struct poulpe_textbuffer));
    if (!textbuffer)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate textbuffer");
        return NULL;
    }

    textbuffer->eof = POULPE_TEXTBUFFER_EOF_LF;

    textbuffer->tree = NULL;

    return textbuffer;
}

enum poulpe_error poulpe_textbuffer_open_file(struct poulpe_textbuffer * textbuffer, const char *path)
{   
    uint8_t *buffer;
    uint32_t buffer_size;

    FILE *fd = fopen(path, "r");
    if (!fd)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, path);
        return POULPE_ERROR_MEMORY;
    }

    fseek(fd, 0, SEEK_END);
    buffer_size = ftell(fd);

    buffer = malloc(buffer_size * sizeof(uint8_t));
    if (buffer == NULL)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate memory");
        return POULPE_ERROR_MEMORY;
    }

    rewind(fd);
    fread(buffer, 1, buffer_size, fd);
    fclose(fd);

    textbuffer->text = poulpe_text_new();
    if (!textbuffer->text)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate text");
        return POULPE_ERROR_MEMORY;
    }

    uint32_t i = 0;
    uint32_t j = 0;
    while(i < buffer_size)
    {
        if (buffer[i] == '\n')
        {
            poulpe_line line = poulpe_line_new((const char *)buffer + j, (const char *)buffer + i + 1);
            if (!line)
            {
                POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate line");
                return POULPE_ERROR_MEMORY;
            }

            textbuffer->text = poulpe_text_push_back(textbuffer->text, &line);
            if (!textbuffer->text)
            {
                POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back line");
                return POULPE_ERROR_MEMORY;
            }

            j = i + 1;
        }

        i++;
    }

    poulpe_line line = poulpe_line_new((const char *)buffer + j, (const char *)buffer + i);
    if (!line)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate line");
        return POULPE_ERROR_MEMORY;
    }

    textbuffer->text = poulpe_text_push_back(textbuffer->text, &line);
    if (!textbuffer->text)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back line");
        return POULPE_ERROR_MEMORY;
    }

    textbuffer->filename = sake_string_new(strrchr(path, '/') ? strrchr(path, '/') + 1 : path, NULL);
    if (!textbuffer->filename)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to save filename");
        return POULPE_ERROR_MEMORY;
    }

    textbuffer->language_type = poulpe_language_auto_detect(textbuffer->filename);

    enum poulpe_error error = _init_parser(textbuffer);
    if (error != POULPE_ERROR_NONE)
        return error;

    return POULPE_ERROR_NONE;
}

void poulpe_textbuffer_free(struct poulpe_textbuffer * textbuffer)
{
    if (textbuffer->filename)
        sake_string_free(textbuffer->filename);
    if (textbuffer->text)
        poulpe_text_free(textbuffer->text);

    if (textbuffer->cursor)
        ts_query_cursor_delete(textbuffer->cursor);
    if (textbuffer->query)
        ts_query_delete(textbuffer->query);
    if (textbuffer->tree) 
        ts_tree_delete(textbuffer->tree);
    if (textbuffer->parser)
        ts_parser_delete(textbuffer->parser);
    
    free(textbuffer);
}

void poulpe_textbuffer_parse(struct poulpe_textbuffer *textbuffer)
{
    ts_parser_reset(textbuffer->parser);
    
    TSInput input = {
        .encoding = TSInputEncodingUTF8,
        .payload = textbuffer,
        .read = _read
    };

    textbuffer->tree = ts_parser_parse(textbuffer->parser, NULL, input);
}

uint32_t poulpe_textbuffer_eof_size(struct poulpe_textbuffer * textbuffer, poulpe_line line)
{
    uint32_t raw_size = poulpe_line_raw_size(line);
    if (raw_size && textbuffer->eof == POULPE_TEXTBUFFER_EOF_LF && line[raw_size - 1] == '\n')
        return raw_size - 1;
    return raw_size;
}

static const char * _read(void *payload, uint32_t byte_index, TSPoint position, uint32_t *bytes_read)
{
    SAKE_MACRO_UNUSED(byte_index);
    struct poulpe_textbuffer *textbuffer = (struct poulpe_textbuffer*) payload;
    poulpe_text text = textbuffer->text;
    *bytes_read = position.row < poulpe_text_size(text) && position.column < poulpe_line_raw_size(text[position.row]) ? strlen(text[position.row] + position.column) : 0;
    return text[position.row] + position.column;
}

static enum poulpe_error _init_parser(struct poulpe_textbuffer *textbuffer)
{
    textbuffer->parser = ts_parser_new();

    ts_parser_set_language(textbuffer->parser, poulpe_language_parser(textbuffer->language_type));

    const char *highlight_query = poulpe_language_query(textbuffer->language_type);
    if (!highlight_query)
        return POULPE_ERROR_NONE;

    TSQueryError query_error;
    uint32_t error_offset = 0;
    textbuffer->query = ts_query_new(poulpe_language_parser(textbuffer->language_type), highlight_query, strlen(highlight_query), &error_offset, &query_error);
    if (query_error != TSQueryErrorNone)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_VALUE, "Query error");
        return POULPE_ERROR_VALUE;
    }

    textbuffer->cursor = ts_query_cursor_new();

    return POULPE_ERROR_NONE;
}