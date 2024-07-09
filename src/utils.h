#ifndef ASYNCINPUT_UTILS_H
#define ASYNCINPUT_UTILS_H

#include <driver/gpio.h>

#include "types.h"

bool prepare_pin(AsyncInput::pin_config_t &pin_cfg);

#endif //ASYNCINPUT_UTILS_H
