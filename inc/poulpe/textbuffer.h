#ifndef POULPE_TEXTBUFFER_H
#define POULPE_TEXTBUFFER_H

#include <stdbool.h>
#include <stdint.h>

#include <sake/string.h>

#include "poulpe/text.h"
#include "poulpe/error.h"
#include "poulpe/language.h"

typedef struct TSParser TSParser;
typedef struct TSTree TSTree;
typedef struct TSQuery TSQuery;
typedef struct TSQueryCursor TSQueryCursor;

struct poulpe_history;

enum poulpe_textbuffer_eof
{
    POULPE_TEXTBUFFER_EOF_LF
};

struct poulpe_textbuffer
{
    poulpe_text text;
    sake_string filename;
    sake_string path;
    enum poulpe_language_type language_type;
    bool dirty;
    enum poulpe_textbuffer_eof eof;
    TSParser *parser;
    TSTree *tree;
    TSQuery *query;
    TSQueryCursor *cursor;
    struct poulpe_history *history;
};

struct poulpe_textbuffer * poulpe_textbuffer_new(void);
void poulpe_textbuffer_free(struct poulpe_textbuffer * textbuffer);
enum poulpe_error poulpe_textbuffer_open_file(struct poulpe_textbuffer * textbuffer, const char *path);
void poulpe_textbuffer_tree_parse(struct poulpe_textbuffer *textbuffer);
void poulpe_textbuffer_tree_edit(struct poulpe_textbuffer *textbuffer);

enum poulpe_error poulpe_textbuffer_undo(struct poulpe_textbuffer *textbuffer);
enum poulpe_error poulpe_textbuffer_redo(struct poulpe_textbuffer *textbuffer);

enum poulpe_error poulpe_textbuffer_new_action(struct poulpe_textbuffer *textbuffer);

enum poulpe_error poulpe_textbuffer_text_push_back(struct poulpe_textbuffer *textbuffer);
enum poulpe_error poulpe_textbuffer_text_insert(struct poulpe_textbuffer *textbuffer, uint32_t line_index);
void poulpe_textbuffer_text_pop_back(struct poulpe_textbuffer *textbuffer);
void poulpe_textbuffer_text_erase(struct poulpe_textbuffer *textbuffer, uint32_t line_index);

const char *poulpe_textbuffer_text_at(struct poulpe_textbuffer *textbuffer, uint32_t line_index);
uint32_t poulpe_textbuffer_text_size(struct poulpe_textbuffer *textbuffer);

enum poulpe_error poulpe_textbuffer_line_push_back(struct poulpe_textbuffer *textbuffer, uint32_t line_index, const char *begin, const char *end);
enum poulpe_error poulpe_textbuffer_line_insert(struct poulpe_textbuffer *textbuffer, uint32_t line_index, uint32_t index, const char *begin, const char *end);
void poulpe_textbuffer_line_pop_back(struct poulpe_textbuffer *textbuffer, uint32_t line_index);
void poulpe_textbuffer_line_erase(struct poulpe_textbuffer *textbuffer, uint32_t line_index, uint32_t index);
void poulpe_textbuffer_line_erase_range(struct poulpe_textbuffer *textbuffer, uint32_t line_index, uint32_t from, uint32_t to);

uint32_t poulpe_textbuffer_line_eof_size(struct poulpe_textbuffer * textbuffer, uint32_t line_index);
uint32_t poulpe_textbuffer_line_raw_size(struct poulpe_textbuffer *textbuffer, uint32_t line_index);
uint32_t poulpe_textbuffer_line_utf8_size(struct poulpe_textbuffer *textbuffer, uint32_t line_index);

uint32_t poulpe_textbuffer_line_utf8_index(struct poulpe_textbuffer *textbuffer, uint32_t line_index, uint32_t raw_index);
uint32_t poulpe_textbuffer_line_raw_index(struct poulpe_textbuffer *textbuffer, uint32_t line_index, uint32_t utf8_index);

#endif /* POULPE_TEXTBUFFER_H */