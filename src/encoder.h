#ifndef ASYNCINPUT_ENCODER_H
#define ASYNCINPUT_ENCODER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>

#include <driver/gpio.h>
#include <driver/pcnt.h>
#include <soc/pcnt_struct.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portable.h>
#include <freertos/semphr.h>

#include "types.h"


class Encoder {
public:
    void tick();
    int32_t get_counter() const;
    void reset_counter();
    void set_handler(AsyncInput::encoder_handler_t handler, void *handler_context = nullptr);

    bool enable(AsyncInput::encoder_config_t config);
    void disable();

protected:
    static Encoder *encoders[PCNT_UNIT_MAX];
    static bool _attachedInterrupt;
    pcnt_config_t _pcnt_config;
    AsyncInput::encoder_config_t _cfg;
    uint32_t _last_rotation_time_us;
    volatile int32_t _counter = 0;
    bool _enabled = false;
    QueueHandle_t _events_queue = nullptr;
    void *_handler_context = nullptr;
    AsyncInput::encoder_handler_t _handler = nullptr;
    void IRAM_ATTR _interrupt();
};

#endif //ASYNCINPUT_ENCODER_H