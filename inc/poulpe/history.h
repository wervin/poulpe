#ifndef POULPE_HISTORY_H
#define POULPE_HISTORY_H

#include <stdint.h>

#include <cimgui.h>

#include "poulpe/error.h"
#include "poulpe/text.h"

enum poulpe_subaction_type
{
    POULPE_SUBACTION_TYPE_TEXT_INSERT,
    POULPE_SUBACTION_TYPE_TEXT_ERASE,
    POULPE_SUBACTION_TYPE_LINE_INSERT,
    POULPE_SUBACTION_TYPE_LINE_ERASE
};

struct poulpe_subaction
{
    enum poulpe_subaction_type type;
    poulpe_line line;
    uint32_t line_index;
    uint32_t from;
    uint32_t to;
};

struct poulpe_action_state
{
    ImVec2 cursor_position;
    ImVec2 selection_start_position;
    ImVec2 selection_end_position;
};

struct poulpe_action
{
    struct poulpe_subaction **subactions;
    struct poulpe_action_state before;
    struct poulpe_action_state after;
};

struct poulpe_history
{
    struct poulpe_action **actions;
    uint32_t current;
};

struct poulpe_subaction *poulpe_subaction_new(void);
void poulpe_subaction_free(struct poulpe_subaction *subaction);

struct poulpe_action *poulpe_action_new(void);
void poulpe_action_free(struct poulpe_action *action);
enum poulpe_error poulpe_action_push_back(struct poulpe_action *action, struct poulpe_subaction **subaction);
uint32_t poulpe_action_size(struct poulpe_action *action);

struct poulpe_history *poulpe_history_new(void);
void poulpe_history_free(struct poulpe_history *history);
uint32_t poulpe_history_size(struct poulpe_history *history);
void poulpe_history_update(struct poulpe_history *history);
enum poulpe_error poulpe_history_push_back(struct poulpe_history *history, struct poulpe_action **action);
struct poulpe_action *poulpe_history_back(struct poulpe_history *history);
struct poulpe_action *poulpe_history_current(struct poulpe_history *history);
void poulpe_history_undo(struct poulpe_history *history);
void poulpe_history_redo(struct poulpe_history *history);

// struct poulpe_subaction

#endif /* POULPE_HISTORY_H */