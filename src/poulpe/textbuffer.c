#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include <sake/utils.h>

#include "poulpe/textbuffer.h"
#include "poulpe/log.h"
#include "poulpe/theme.h"

extern const TSLanguage *tree_sitter_json(void);

// static void _colorize(struct poulpe_textbuffer *textbuffer);

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
            poulpe_line line = poulpe_line_new_range((const char *)buffer + j, (const char *)buffer + i);
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

    poulpe_line line = poulpe_line_new_range((const char *)buffer + j, (const char *)buffer + i);
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

    textbuffer->filename = sake_string_new(strrchr(path, '/') ? strrchr(path, '/') + 1 : path);
    if (!textbuffer->filename)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to save filename");
        return POULPE_ERROR_MEMORY;
    }

    // _colorize(textbuffer);

    return POULPE_ERROR_NONE;
}

void poulpe_textbuffer_free(struct poulpe_textbuffer * textbuffer)
{
    if (textbuffer->filename)
        sake_string_free(textbuffer->filename);
    if (textbuffer->text)
        poulpe_text_free(textbuffer->text);
    free(textbuffer);
}

// const char* poulpe_textbuffer_line_at(struct poulpe_textbuffer * textbuffer, uint32_t line_index)
// {   
//     return textbuffer->text[line_index];
// }

// uint32_t poulpe_textbuffer_line_textsize(struct poulpe_textbuffer * textbuffer, uint32_t line_index)
// {
//     ImVec2 line_size;
//     ImFont_CalcTextSizeA(&line_size, igGetFont(), igGetFontSize(), FLT_MAX, -1.0f, textbuffer->text[line_index], NULL, NULL);
//     return line_size.x;
// }

// uint32_t poulpe_textbuffer_line_raw_size(struct poulpe_textbuffer * textbuffer, uint32_t line_index)
// {
//     return sake_string_raw_size(textbuffer->text[line_index]);
// }

// uint32_t poulpe_textbuffer_glyph_utf8_length(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t glyph_index)
// {
//     return sake_utils_utf8_length(textbuffer->text[line_index][glyph_index]);
// }


// uint32_t poulpe_textbuffer_text_size(struct poulpe_textbuffer * textbuffer)
// {
//     return poulpe_text_size(textbuffer->text);
// }

// enum poulpe_error poulpe_textbuffer_new_line(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t glyph_index)
// {
//     poulpe_line line = poulpe_line_new();
//     if (!line)
//     {
//         POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate line");
//         return POULPE_ERROR_MEMORY;
//     }

//     line = poulpe_line_copy(textbuffer->text[line_index], line);

//     for (uint32_t i = 0; i < glyph_index; i++)
//         poulpe_line_erase(textbuffer->text[line_index], 0);

//     for (uint32_t i = poulpe_line_size(line); i > glyph_index; i--)
//         poulpe_line_pop_back(line);

//     enum poulpe_error error =  poulpe_textbuffer_text_insert(textbuffer, line_index, &line);
//     if (error != POULPE_ERROR_NONE)
//         return error;
    
//     return POULPE_ERROR_NONE;
// }

// enum poulpe_error poulpe_textbuffer_text_push_back(struct poulpe_textbuffer * textbuffer, poulpe_line *line)
// {
//     textbuffer->text = poulpe_text_push_back(textbuffer->text, line);
//     if (!textbuffer->text)
//     {
//         POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back line");
//         return POULPE_ERROR_MEMORY;
//     }
//     return POULPE_ERROR_NONE;
// }

// enum poulpe_error poulpe_textbuffer_text_insert(struct poulpe_textbuffer * textbuffer, uint32_t line_index, poulpe_line *line)
// {
//     textbuffer->text = poulpe_text_insert(textbuffer->text, line_index, line);
//     if (!textbuffer->text)
//     {
//         POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to insert line");
//         return POULPE_ERROR_MEMORY;
//     }
//     return POULPE_ERROR_NONE;
// }

// void poulpe_textbuffer_text_erase(struct poulpe_textbuffer * textbuffer, uint32_t line_index)
// {
//     poulpe_text_erase(textbuffer->text, line_index);
// }

// uint32_t poulpe_textbuffer_line_size(struct poulpe_textbuffer * textbuffer, uint32_t line_index)
// {
//     return poulpe_line_size(textbuffer->text[line_index]);
//     return 0;
// }

// float poulpe_textbuffer_line_full_textsize(struct poulpe_textbuffer * textbuffer, uint32_t line_index)
// {
//     return poulpe_line_full_textsize(textbuffer->text[line_index]);
//     return 0;
// }

// float poulpe_textbuffer_line_subset_textsize(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t start, uint32_t end)
// {
//     return poulpe_line_subset_textsize(textbuffer->text[line_index], start, end);
//     return 0;
// }

// enum poulpe_error poulpe_textbuffer_line_push_back(struct poulpe_textbuffer * textbuffer, uint32_t line_index, struct poulpe_glyph *glyph)
// {
//     textbuffer->text[line_index] = poulpe_line_push_back(textbuffer->text[line_index], glyph);
//     if (!textbuffer->text[line_index])
//     {
//         POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back glyph");
//         return POULPE_ERROR_MEMORY;
//     }
//     return POULPE_ERROR_NONE;
// }

// enum poulpe_error poulpe_textbuffer_line_insert(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t glyph_index, struct poulpe_glyph *glyph)
// {
//     textbuffer->text[line_index] = poulpe_line_insert(textbuffer->text[line_index], glyph_index, glyph);
//     if (!textbuffer->text[line_index])
//     {
//         POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to insert glyph");
//         return POULPE_ERROR_MEMORY;
//     }
//     return POULPE_ERROR_NONE;
// }

// void poulpe_textbuffer_line_erase(struct poulpe_textbuffer * textbuffer, uint32_t line_index, uint32_t glyph_index)
// {
//     poulpe_line_erase(textbuffer->text[line_index], glyph_index);
// }

// struct poulpe_glyph *poulpe_textbuffer_line_at(struct poulpe_textbuffer *textbuffer, uint32_t line_index, uint32_t glyph_index)
// {
//     return &textbuffer->text[line_index][glyph_index];
// }

// static const char * _read(void *payload, uint32_t byte_index, TSPoint position, uint32_t *bytes_read)
// {
//     printf("byte_index: %u\n", byte_index);    
//     printf("position.row: %u, position.column: %u\n", position.row, position.column);
//     poulpe_text text = (poulpe_text) payload;
//     *bytes_read = poulpe_line_size(text[position.column]);
//     return payload;
// }

// static void _colorize(struct poulpe_textbuffer *textbuffer)
// {
//     TSParser *parser = ts_parser_new();

//     ts_parser_set_language(parser, tree_sitter_json());

//     TSInput input = {
//         .encoding = TSInputEncodingUTF8,
//         .payload = textbuffer->text,
//         .read = _read
//     };

//     TSTree *tree = ts_parser_parse(parser, NULL, input);

//     TSNode root_node = ts_tree_root_node(tree);
//     char *string = ts_node_string(root_node);
//     printf("\nSyntax tree: %s\n\n", string);

//     // const char highlight_query[] = 
//     // "(pair key: (_) @string.special.key) "
//     // "(string) @string "
//     // "(number) @number "
//     // "[(null) (true) (false)] @constant.builtin "
//     // "(escape_sequence) @escape "
//     // "(comment) @comment";
//     // TSQueryError query_error;
//     // uint32_t error_offset = 0;
//     // TSQuery *query = ts_query_new(tree_sitter_json(), highlight_query, strlen(highlight_query), &error_offset, &query_error);
//     // if (query_error != TSQueryErrorNone)
//     // {
//     //     POULPE_LOG_ERROR(POULPE_ERROR_VALUE, "Query error");
//     //     goto end;
//     // }

//     // TSQueryCursor *cursor = ts_query_cursor_new();

//     // ts_query_cursor_exec(cursor, query, ts_tree_root_node(tree));

//     // uint32_t cursor_start = 0;
//     // uint32_t cursor_end = 0;
//     // TSQueryMatch match = {0};
//     // while (ts_query_cursor_next_match(cursor, &match))
//     // {
//     //     // cursor_start = ts_node_start_byte(match.captures[0].node);
//     //     // if (cursor_start < cursor_end)
//     //     //     continue;

//     //     // for (uint32_t i = cursor_end; i < cursor_start; i++)
//     //     //     line[i].color = igColorConvertFloat4ToU32(theme->text);

//     //     // cursor_end = ts_node_end_byte(match.captures[0].node);

//     //     // for (uint32_t i = cursor_start; i < cursor_end; i++)
//     //     //     line[i].color = igColorConvertFloat4ToU32(theme->keyword);
//     // }

//     // // for (uint32_t i = cursor_end; i < strlen(str_line); i++)
//     // //     line[i].color = igColorConvertFloat4ToU32(theme->text);

//     // ts_query_cursor_delete(cursor);
//     // ts_query_delete(query);

// // end:
//     free(string);
//     ts_tree_delete(tree);
//     ts_parser_delete(parser);
// }