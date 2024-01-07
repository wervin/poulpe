#include <stdlib.h>

#include <sake/vector.h>

#include "poulpe/history.h"

#include "poulpe/log.h"

struct poulpe_subaction *poulpe_subaction_new(void)
{
    struct poulpe_subaction *subaction;

    subaction = calloc(1, sizeof(struct poulpe_subaction));
    if (!subaction)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate subaction");
        return NULL;
    }

    subaction->line = NULL;

    return subaction;
}

void poulpe_subaction_free(struct poulpe_subaction *subaction)
{
    if (subaction->line)
        poulpe_line_free(subaction->line);
    free(subaction);
}

struct poulpe_action *poulpe_action_new(void)
{
    struct poulpe_action *action;

    action = calloc(1, sizeof(struct poulpe_action));
    if (!action)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate action");
        return NULL;
    }

    action->subactions = sake_vector_new(sizeof(struct poulpe_subaction *), (void (*)(void *))poulpe_subaction_free);

    return action;
}

void poulpe_action_free(struct poulpe_action *action)
{
    sake_vector_free(action->subactions);
    free(action);
}

enum poulpe_error poulpe_action_push_back(struct poulpe_action *action, struct poulpe_subaction **subaction)
{
    action->subactions = sake_vector_push_back(action->subactions, subaction);
    if (!action->subactions)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back subaction");
        return POULPE_ERROR_MEMORY;
    }
    return POULPE_ERROR_NONE;
}

uint32_t poulpe_action_size(struct poulpe_action *action)
{
    return sake_vector_size(action->subactions);
}

struct poulpe_history *poulpe_history_new(void)
{
    struct poulpe_history *history;

    history = calloc(1, sizeof(struct poulpe_history));
    if (!history)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate history");
        return NULL;
    }

    history->current = 0;
    history->actions = sake_vector_new(sizeof(struct poulpe_action *), (void (*)(void *))poulpe_action_free);

    return history;
}

void poulpe_history_free(struct poulpe_history *history)
{
    sake_vector_free(history->actions);
    free(history);
}

uint32_t poulpe_history_size(struct poulpe_history *history)
{
    return sake_vector_size(history->actions);
}

void poulpe_history_update(struct poulpe_history *history)
{
    uint32_t size = sake_vector_size(history->actions);
    if (!size)
        return;

    if (history->current == size)
        return;

    sake_vector_erase_range(history->actions, history->current, size);
}

enum poulpe_error poulpe_history_push_back(struct poulpe_history *history, struct poulpe_action **action)
{
    history->actions = sake_vector_push_back(history->actions, action);
    if (!history->actions)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back action");
        return POULPE_ERROR_MEMORY;
    }
    
    history->current = sake_vector_size(history->actions);
    return POULPE_ERROR_NONE;
}

struct poulpe_action *poulpe_history_back(struct poulpe_history *history)
{
    uint32_t size = sake_vector_size(history->actions);
    return history->actions[size - 1];
}

struct poulpe_action *poulpe_history_current(struct poulpe_history *history)
{
    if (!history->current)
        return NULL;
    return history->actions[history->current - 1];
}

void poulpe_history_undo(struct poulpe_history *history)
{
    if (history->current > 0)
        history->current--;
}

void poulpe_history_redo(struct poulpe_history *history)
{
    uint32_t size = sake_vector_size(history->actions);
    if (history->current < size)
        history->current++;
}
