#ifndef POULPE_HISTORY_H
#define POULPE_HISTORY_H

#include <stdint.h>

enum poulpe_subaction_type
{
    POULPE_SUBACTION_TYPE_PUSH,
    POULPE_SUBACTION_TYPE_ERASE
};

struct poulpe_subaction
{
    enum poulpe_subaction_type type;
};

struct poulpe_action
{
    struct poulpe_subaction *subactions;
};

struct poulpe_history
{
    struct poulpe_action *actions;
    uint32_t index;
};

// struct poulpe_subaction

#endif /* POULPE_HISTORY_H */