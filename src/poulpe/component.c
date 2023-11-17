#include "poulpe/component.h"

#include "poulpe/log.h"

#define X(__def, __id, __type)                                                                                                           \
    struct poulpe_##__type;                                                                                                              \
    extern struct poulpe_##__type *poulpe_##__type##_new(void);                                                                          \
    extern enum poulpe_error poulpe_##__type##_notify(struct poulpe_##__type *component, struct poulpe_event *event); \
    extern enum poulpe_error poulpe_##__type##_draw(struct poulpe_##__type *component);                                                  \
    extern void poulpe_##__type##_free(struct poulpe_##__type *component);

POULPE_COMPONENTS

#undef X

struct poulpe_component *poulpe_component_new(enum poulpe_component_type type)
{
    struct poulpe_component *component;

    switch (type)
    {
#define X(__def, __id, __type)                                          \
    case __id:                                                          \
        component = (struct poulpe_component *)poulpe_##__type##_new(); \
        component->parent = NULL;                                       \
        component->type = __id;                                         \
        return component;

        POULPE_COMPONENTS

#undef X

    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown component type");
        return NULL;
    }
}

enum poulpe_error poulpe_component_draw(struct poulpe_component *component)
{
    switch (component->type)
    {
#define X(__def, __id, __type) \
    case __id:                 \
        return poulpe_##__type##_draw((struct poulpe_##__type *)component);

        POULPE_COMPONENTS

#undef X

    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown component type");
        return POULPE_ERROR_UNKNOWN;
    }
}

enum poulpe_error poulpe_component_notify(struct poulpe_component *component, struct poulpe_event *event)
{
    switch (component->type)
    {
#define X(__def, __id, __type) \
    case __id:                 \
        return poulpe_##__type##_notify((struct poulpe_##__type *)component, event);

        POULPE_COMPONENTS

#undef X

    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown component type");
        return POULPE_ERROR_UNKNOWN;
    }
}

void poulpe_component_free(struct poulpe_component *component)
{
    switch (component->type)
    {
#define X(__def, __id, __type)                                       \
    case __id:                                                       \
        poulpe_##__type##_free((struct poulpe_##__type *)component); \
        break;

        POULPE_COMPONENTS

#undef X

    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown component type");
    }
}