#ifndef POULPE_DISPLAY_H
#define POULPE_DISPLAY_H

#include <stdint.h>

#include "poulpe/error.h"

enum poulpe_error poulpe_display_init(void);
void poulpe_display_destroy(void);

#endif /* POULPE_DISPLAY_H */