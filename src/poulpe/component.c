#include <string.h>

#include "poulpe/component.h"

#include "poulpe/log.h"

#define X(def, id, type)                                                                                                 \
    struct poulpe_##type;                                                                                                \
    extern enum poulpe_error poulpe_##type##_update(struct poulpe_##type *component, vec2 upper_left, vec2 lower_right); \
    extern enum poulpe_error poulpe_##type##_draw(struct poulpe_##type *component);                                      \
    extern void poulpe_##type##_destroy(struct poulpe_##type *component);

POULPE_COMPONENTS

#undef X


enum poulpe_error poulpe_component_update(struct poulpe_component *component, vec2 upper_left, vec2 lower_right)
{
    switch (component->type)
    {
#define X(def, id, type)                                           \
    case id:                                                       \
        component->x = upper_left[0];                              \
        component->y = upper_left[1];                              \
        component->width = lower_right[0] - upper_left[0];         \
        component->height = lower_right[1] - upper_left[1];        \
        memcpy(component->upper_left, upper_left, sizeof(vec2));   \
        memcpy(component->lower_right, lower_right, sizeof(vec2)); \
        return poulpe_##type##_update((struct poulpe_##type *)component, upper_left, lower_right);

        POULPE_COMPONENTS

#undef X

    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown component type");
        return POULPE_ERROR_UNKNOWN;
    }
}

enum poulpe_error poulpe_component_draw(struct poulpe_component *component)
{
    switch (component->type)
    {
#define X(def, id, type) \
    case id:             \
        return poulpe_##type##_draw((struct poulpe_##type *)component);

        POULPE_COMPONENTS

#undef X

    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown component type");
        return POULPE_ERROR_UNKNOWN;
    }
}

void poulpe_component_destroy(struct poulpe_component *component)
{
    switch (component->type)
    {
#define X(def, id, type)                                            \
    case id:                                                        \
        poulpe_##type##_destroy((struct poulpe_##type *)component); \
        break;

        POULPE_COMPONENTS

#undef X

    default:
        POULPE_LOG_ERROR(POULPE_ERROR_UNKNOWN, "Unknown component type");
    }
}