#ifndef POULPE_IO_H
#define POULPE_IO_H

#include <stdbool.h>

#include "poulpe/component.h"

enum poulpe_error poulpe_io_handle_keyboard(struct poulpe_component *component);
enum poulpe_error poulpe_io_handle_mouse(struct poulpe_component *component);

#endif /* POULPE_IO_H */