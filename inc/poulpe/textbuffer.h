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
};

struct poulpe_textbuffer * poulpe_textbuffer_new(void);
void poulpe_textbuffer_free(struct poulpe_textbuffer * textbuffer);
enum poulpe_error poulpe_textbuffer_open_file(struct poulpe_textbuffer * textbuffer, const char *path);
void poulpe_textbuffer_tree_parse(struct poulpe_textbuffer *textbuffer);
void poulpe_textbuffer_tree_edit(struct poulpe_textbuffer *textbuffer);
uint32_t poulpe_textbuffer_eof_size(struct poulpe_textbuffer * textbuffer, poulpe_line line);

#endif /* POULPE_TEXTBUFFER_H */