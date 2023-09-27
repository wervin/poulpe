#ifndef POULPE_EDITOR_H
#define POULPE_EDITOR_H

#include <stdint.h>

#include "poulpe/error.h"

enum poulpe_error poulpe_editor_init(void);
enum poulpe_error poulpe_editor_draw(void);
void poulpe_editor_destroy(void);

#endif /* POULPE_EDITOR_H */