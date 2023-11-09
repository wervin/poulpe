#include "poulpe/component.h"

#include "poulpe/components/layout.h"
#include "poulpe/components/rectangle.h"

#include "poulpe/log.h"

enum poulpe_error poulpe_component_update(struct poulpe_component *component, vec2 upper_left, vec2 lower_right)
{
    switch (component->type)
    {
#define X(def, id, type) \
    case id:             \
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