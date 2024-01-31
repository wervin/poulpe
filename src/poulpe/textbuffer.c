#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include <sake/utils.h>
#include <sake/macro.h>
#include <sake/vector.h>

#include <tree_sitter/api.h>

#include "poulpe/textbuffer.h"
#include "poulpe/log.h"
#include "poulpe/style.h"
#include "poulpe/text.h"
#include "poulpe/language.h"
#include "poulpe/history.h"

static const char * _read(void *payload, uint32_t byte_index, TSPoint position, uint32_t *bytes_read);

static enum poulpe_error _undo_subaction(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction);
static enum poulpe_error _redo_subaction(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction);

static enum poulpe_error _undo_text_insert(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction);
static enum poulpe_error _redo_text_insert(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction);

static enum poulpe_error _undo_text_erase(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction);
static enum poulpe_error _redo_text_erase(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction);

static enum poulpe_error _undo_line_insert(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction);
static enum poulpe_error _redo_line_insert(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction);

static enum poulpe_error _undo_line_erase(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction);
static enum poulpe_error _redo_line_erase(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction);

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

    textbuffer->history = poulpe_history_new();
    if (!textbuffer->history)
        return NULL;

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

    textbuffer->path = sake_string_new(path, NULL);
    if (!textbuffer->path)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to save path");
        return POULPE_ERROR_MEMORY;
    }

    enum poulpe_error error = poulpe_textbuffer_set_language(textbuffer, poulpe_language_auto_detect(textbuffer->filename));
    if (error != POULPE_ERROR_NONE)
        return error;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textbuffer_save_file(struct poulpe_textbuffer * textbuffer)
{
    FILE *fd = fopen(textbuffer->path, "w");
    if (!fd)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, textbuffer->path);
        return POULPE_ERROR_MEMORY;
    }

    for (uint32_t i = 0; i < poulpe_text_size(textbuffer->text); i++)
    {
        fwrite(textbuffer->text[i], sizeof(char), poulpe_line_raw_size(textbuffer->text[i]), fd);
    }

    fclose(fd);
    return POULPE_ERROR_NONE;
}

void poulpe_textbuffer_free(struct poulpe_textbuffer * textbuffer)
{
    if (textbuffer->filename)
        sake_string_free(textbuffer->filename);
    if (textbuffer->path)
        sake_string_free(textbuffer->path);
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

    poulpe_history_free(textbuffer->history);
    free(textbuffer);
}

void poulpe_textbuffer_tree_parse(struct poulpe_textbuffer *textbuffer)
{
    if (!textbuffer->parser)
        return;

    TSInput input = {
        .encoding = TSInputEncodingUTF8,
        .payload = textbuffer,
        .read = _read
    };

    if (textbuffer->tree)
        ts_tree_delete(textbuffer->tree);

    textbuffer->tree = ts_parser_parse(textbuffer->parser, NULL, input);
}

void poulpe_textbuffer_tree_edit(struct poulpe_textbuffer *textbuffer)
{
    poulpe_textbuffer_tree_parse(textbuffer);
}

enum poulpe_error poulpe_textbuffer_set_language(struct poulpe_textbuffer *textbuffer, enum poulpe_language_type language)
{
    if (textbuffer->cursor)
    {
        ts_query_cursor_delete(textbuffer->cursor);
        textbuffer->cursor = NULL;
    }
    if (textbuffer->query)
    {
        ts_query_delete(textbuffer->query);
        textbuffer->query = NULL;
    }
    if (textbuffer->tree)
    {
        ts_tree_delete(textbuffer->tree);
        textbuffer->tree = NULL;
    }
    if (textbuffer->parser)
    {
        ts_parser_delete(textbuffer->parser);
        textbuffer->parser = NULL;
    }

    if (language == POULPE_LANGUAGE_TYPE_RAW)
        goto end;

    textbuffer->parser = ts_parser_new();

    ts_parser_set_language(textbuffer->parser, poulpe_language_parser(language));

    const char *highlight_query = poulpe_language_query(language);
    if (!highlight_query)
        return POULPE_ERROR_NONE;

    TSQueryError query_error;
    uint32_t error_offset = 0;
    textbuffer->query = ts_query_new(poulpe_language_parser(language), highlight_query, strlen(highlight_query), &error_offset, &query_error);
    if (query_error != TSQueryErrorNone)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_VALUE, "Query error");
        return POULPE_ERROR_VALUE;
    }

    textbuffer->cursor = ts_query_cursor_new();

    poulpe_textbuffer_tree_parse(textbuffer);

end:
    textbuffer->language_type = language;
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textbuffer_undo(struct poulpe_textbuffer *textbuffer)
{
    if (!poulpe_history_size(textbuffer->history))
        return POULPE_ERROR_NONE;

    if (textbuffer->history->current)
    {
        struct poulpe_action *action = poulpe_history_current(textbuffer->history);
        for (uint32_t i = poulpe_action_size(action); i > 0; i--)
        {
            enum poulpe_error error = _undo_subaction(textbuffer, action->subactions[i - 1]);
            if (error != POULPE_ERROR_NONE)
                return error;
        }
        
        poulpe_history_undo(textbuffer->history);
    }

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textbuffer_redo(struct poulpe_textbuffer *textbuffer)
{
    if (!poulpe_history_size(textbuffer->history))
        return POULPE_ERROR_NONE;

    if (textbuffer->history->current < poulpe_history_size(textbuffer->history))
    {
        poulpe_history_redo(textbuffer->history);

        struct poulpe_action *action = poulpe_history_current(textbuffer->history);
        for (uint32_t i = 0; i < poulpe_action_size(action); i++)
        {
            enum poulpe_error error = _redo_subaction(textbuffer, action->subactions[i]);
            if (error != POULPE_ERROR_NONE)
                return error;
        }
    }
    
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textbuffer_new_action(struct poulpe_textbuffer *textbuffer)
{
    struct poulpe_action *action = poulpe_action_new();
    if (!action)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate action");
        return POULPE_ERROR_MEMORY;
    }

    poulpe_history_update(textbuffer->history);

    enum poulpe_error error = poulpe_history_push_back(textbuffer->history, &action);
    if (error != POULPE_ERROR_NONE)
        return error;
    
    return POULPE_ERROR_NONE;
}

const char *poulpe_textbuffer_text_at(struct poulpe_textbuffer *textbuffer, uint32_t line_index)
{
    return textbuffer->text[line_index];
}

enum poulpe_error poulpe_textbuffer_text_insert(struct poulpe_textbuffer *textbuffer, uint32_t line_index)
{
    struct poulpe_subaction *subaction = poulpe_subaction_new();
    if (!subaction)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate subaction");
        return POULPE_ERROR_MEMORY;
    }

    subaction->type = POULPE_SUBACTION_TYPE_TEXT_INSERT;
    subaction->line = NULL;
    subaction->line_index = line_index;
    subaction->from = 0;
    subaction->to = 0;

    struct poulpe_action *action = poulpe_history_back(textbuffer->history);

    enum poulpe_error error = poulpe_action_push_back(action, &subaction);
    if (error != POULPE_ERROR_NONE)
        return error;
    
    poulpe_line line = poulpe_line_new("", NULL);
    if (!line)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate line");
        return POULPE_ERROR_MEMORY;
    }

    textbuffer->text = poulpe_text_insert(textbuffer->text, line_index, &line);
    if (!textbuffer->text)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to insert new line");
        return POULPE_ERROR_MEMORY;
    }

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textbuffer_text_erase(struct poulpe_textbuffer *textbuffer, uint32_t line_index)
{
    struct poulpe_subaction *subaction = poulpe_subaction_new();
    if (!subaction)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate subaction");
        return POULPE_ERROR_MEMORY;
    }

    subaction->type = POULPE_SUBACTION_TYPE_TEXT_ERASE;
    subaction->line = poulpe_line_new(textbuffer->text[line_index], NULL);
    subaction->line_index = line_index;
    subaction->from = 0;
    subaction->to = 0;

    struct poulpe_action *action = poulpe_history_back(textbuffer->history);

    enum poulpe_error error = poulpe_action_push_back(action, &subaction);
    if (error != POULPE_ERROR_NONE)
        return error;
    
    poulpe_text_erase(textbuffer->text, line_index);

    return POULPE_ERROR_NONE;
}

bool poulpe_textbuffer_find(struct poulpe_textbuffer *textbuffer, const char *str, bool case_sensitive, ImVec2 *pos)
{

    char *(*_strstr)(const char *, const char *) = case_sensitive ? strstr : strcasestr;

    uint32_t size = poulpe_text_size(textbuffer->text);
    if (pos->x >= size)
        return false;

    uint32_t i = pos->x;
    if (pos->y < poulpe_line_raw_size(textbuffer->text[i]))
    {
        char *ret = _strstr(textbuffer->text[i] + (uint32_t)pos->y, str);
        if (ret)
        {
            pos->x = i;
            pos->y = ret - textbuffer->text[i];
            return true;
        }
    }

    i++;
    while (i < poulpe_text_size(textbuffer->text))
    {
        char *ret = _strstr(textbuffer->text[i], str);
        if (ret)
        {
            pos->x = i;
            pos->y = ret - textbuffer->text[i];
            return true;
        }
        i++;
    }

    return false;
}

uint32_t poulpe_textbuffer_text_size(struct poulpe_textbuffer *textbuffer)
{
    return poulpe_text_size(textbuffer->text);
}

enum poulpe_error poulpe_textbuffer_line_push_back(struct poulpe_textbuffer *textbuffer, uint32_t line_index, const char *begin, const char *end)
{
    uint32_t size = poulpe_line_raw_size(textbuffer->text[line_index]);

    struct poulpe_subaction *subaction = poulpe_subaction_new();
    if (!subaction)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate subaction");
        return POULPE_ERROR_MEMORY;
    }

    subaction->type = POULPE_SUBACTION_TYPE_LINE_INSERT;
    subaction->line = poulpe_line_new(begin, end);
    subaction->line_index = line_index;
    subaction->from = size;
    subaction->to = end ? end - begin : (uint32_t) strlen(begin);
    subaction->to += subaction->from;

    struct poulpe_action *action = poulpe_history_back(textbuffer->history);

    enum poulpe_error error = poulpe_action_push_back(action, &subaction);
    if (error != POULPE_ERROR_NONE)
        return error;
    
    textbuffer->text[line_index] = poulpe_line_push_back(textbuffer->text[line_index], begin, end);
    if (!textbuffer->text[line_index])
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back new characters");
        return POULPE_ERROR_MEMORY;
    }
    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textbuffer_line_insert(struct poulpe_textbuffer *textbuffer, uint32_t line_index, uint32_t index, const char *begin, const char *end)
{
    struct poulpe_subaction *subaction = poulpe_subaction_new();
    if (!subaction)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate subaction");
        return POULPE_ERROR_MEMORY;
    }

    subaction->type = POULPE_SUBACTION_TYPE_LINE_INSERT;
    subaction->line = poulpe_line_new(begin, end);
    subaction->line_index = line_index;
    subaction->from = index;
    subaction->to = end ? end - begin : (uint32_t) strlen(begin);
    subaction->to += subaction->from;

    struct poulpe_action *action = poulpe_history_back(textbuffer->history);

    enum poulpe_error error = poulpe_action_push_back(action, &subaction);
    if (error != POULPE_ERROR_NONE)
        return error;
    
    textbuffer->text[line_index] = poulpe_line_insert(textbuffer->text[line_index], index, begin, end);
    if (!textbuffer->text[line_index])
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to insert new characters");
        return POULPE_ERROR_MEMORY;
    }

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textbuffer_line_erase(struct poulpe_textbuffer *textbuffer, uint32_t line_index, uint32_t index)
{
    struct poulpe_subaction *subaction = poulpe_subaction_new();
    if (!subaction)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate subaction");
        return POULPE_ERROR_MEMORY;
    }

    subaction->type = POULPE_SUBACTION_TYPE_LINE_ERASE;
    subaction->line = poulpe_line_new(textbuffer->text[line_index] + index, textbuffer->text[line_index] + index + 1);
    subaction->line_index = line_index;
    subaction->from = index;
    subaction->to = subaction->from + 1;

    struct poulpe_action *action = poulpe_history_back(textbuffer->history);

    enum poulpe_error error = poulpe_action_push_back(action, &subaction);
    if (error != POULPE_ERROR_NONE)
        return error;
    
    poulpe_line_erase(textbuffer->text[line_index], index);

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textbuffer_line_erase_range(struct poulpe_textbuffer *textbuffer, uint32_t line_index, uint32_t from, uint32_t to)
{
    struct poulpe_subaction *subaction = poulpe_subaction_new();
    if (!subaction)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate subaction");
        return POULPE_ERROR_MEMORY;
    }

    subaction->type = POULPE_SUBACTION_TYPE_LINE_ERASE;
    subaction->line = poulpe_line_new(textbuffer->text[line_index] + from, textbuffer->text[line_index] + to);
    subaction->line_index = line_index;
    subaction->from = from;
    subaction->to += to;

    struct poulpe_action *action = poulpe_history_back(textbuffer->history);

    enum poulpe_error error = poulpe_action_push_back(action, &subaction);
    if (error != POULPE_ERROR_NONE)
        return error;
    
    poulpe_line_erase_range(textbuffer->text[line_index], from, to);

    return POULPE_ERROR_NONE;
}

uint32_t poulpe_textbuffer_line_eof_size(struct poulpe_textbuffer * textbuffer, uint32_t line_index)
{
    uint32_t raw_size = poulpe_line_raw_size(textbuffer->text[line_index]);
    if (raw_size && textbuffer->eof == POULPE_TEXTBUFFER_EOF_LF && textbuffer->text[line_index][raw_size - 1] == '\n')
        return raw_size - 1;
    return raw_size;
}

uint32_t poulpe_textbuffer_line_raw_size(struct poulpe_textbuffer *textbuffer, uint32_t line_index)
{
    return poulpe_line_raw_size(textbuffer->text[line_index]);
}

uint32_t poulpe_textbuffer_line_utf8_size(struct poulpe_textbuffer *textbuffer, uint32_t line_index)
{
    return poulpe_line_utf8_size(textbuffer->text[line_index]);
}

uint32_t poulpe_textbuffer_line_utf8_index(struct poulpe_textbuffer *textbuffer, uint32_t line_index, uint32_t raw_index)
{
    return poulpe_line_utf8_index(textbuffer->text[line_index], raw_index);
}

uint32_t poulpe_textbuffer_line_raw_index(struct poulpe_textbuffer *textbuffer, uint32_t line_index, uint32_t utf8_index)
{
    return poulpe_line_raw_index(textbuffer->text[line_index], utf8_index);
}

static const char * _read(void *payload, uint32_t byte_index, TSPoint position, uint32_t *bytes_read)
{
    SAKE_MACRO_UNUSED(byte_index);
    struct poulpe_textbuffer *textbuffer = (struct poulpe_textbuffer*) payload;
    poulpe_text text = textbuffer->text;
    *bytes_read = position.row < poulpe_text_size(text) && position.column < poulpe_line_raw_size(text[position.row]) ? strlen(text[position.row] + position.column) : 0;
    return text[position.row] + position.column;
}

static enum poulpe_error _undo_subaction(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction)
{
    switch (subaction->type)
    {
    case POULPE_SUBACTION_TYPE_TEXT_INSERT:
        return _undo_text_insert(textbuffer, subaction);

    case POULPE_SUBACTION_TYPE_TEXT_ERASE:
        return _undo_text_erase(textbuffer, subaction);

    case POULPE_SUBACTION_TYPE_LINE_INSERT:
        return _undo_line_insert(textbuffer, subaction);

    case POULPE_SUBACTION_TYPE_LINE_ERASE:
        return _undo_line_erase(textbuffer, subaction);
    
    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown subaction type");
        return POULPE_ERROR_UNKNOWN;
    }
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _redo_subaction(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction)
{
    switch (subaction->type)
    {
    case POULPE_SUBACTION_TYPE_TEXT_INSERT:
        return _redo_text_insert(textbuffer, subaction);

    case POULPE_SUBACTION_TYPE_TEXT_ERASE:
        return _redo_text_erase(textbuffer, subaction);

    case POULPE_SUBACTION_TYPE_LINE_INSERT:
        return _redo_line_insert(textbuffer, subaction);

    case POULPE_SUBACTION_TYPE_LINE_ERASE:
        return _redo_line_erase(textbuffer, subaction);
    
    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown subaction type");
        return POULPE_ERROR_UNKNOWN;
    }
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _undo_text_insert(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction)
{
    poulpe_text_erase(textbuffer->text, subaction->line_index);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _redo_text_insert(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction)
{
    poulpe_line line = poulpe_line_new("", NULL);
    if (!line)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate line");
        return POULPE_ERROR_MEMORY;
    }

    textbuffer->text = poulpe_text_insert(textbuffer->text, subaction->line_index, &line);
    if (!textbuffer->text)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to insert new line");
        return POULPE_ERROR_MEMORY;
    }
    
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _undo_text_erase(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction)
{
    poulpe_line line = poulpe_line_new(subaction->line, NULL);
    if (!line)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate line");
        return POULPE_ERROR_MEMORY;
    }

    textbuffer->text = poulpe_text_insert(textbuffer->text, subaction->line_index, &line);
    if (!textbuffer->text)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to insert new line");
        return POULPE_ERROR_MEMORY;
    }

    return POULPE_ERROR_NONE;
}

static enum poulpe_error _redo_text_erase(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction)
{
    poulpe_text_erase(textbuffer->text, subaction->line_index);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _undo_line_insert(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction)
{
    poulpe_line_erase_range(textbuffer->text[subaction->line_index], subaction->from, subaction->to);
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _redo_line_insert(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction)
{
    textbuffer->text[subaction->line_index] = poulpe_line_insert(textbuffer->text[subaction->line_index], subaction->from, subaction->line, NULL);
    if (!textbuffer->text[subaction->line_index])
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to insert new characters");
        return POULPE_ERROR_MEMORY;
    }
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _undo_line_erase(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction)
{
    textbuffer->text[subaction->line_index] = poulpe_line_insert(textbuffer->text[subaction->line_index], subaction->from, subaction->line, NULL);
    if (!textbuffer->text[subaction->line_index])
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to insert new characters");
        return POULPE_ERROR_MEMORY;
    }
    return POULPE_ERROR_NONE;
}

static enum poulpe_error _redo_line_erase(struct poulpe_textbuffer *textbuffer, struct poulpe_subaction *subaction)
{
    poulpe_line_erase_range(textbuffer->text[subaction->line_index], subaction->from, subaction->to);
    return POULPE_ERROR_NONE;
}
