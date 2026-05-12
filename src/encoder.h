#ifndef ASYNCINPUT_ENCODER_H
#define ASYNCINPUT_ENCODER_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <driver/gpio.h>
#include <driver/pcnt.h>
#include <soc/pcnt_struct.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portable.h>
#include <freertos/semphr.h>

#include "types.h"


class EncoderCore {
public:
    void tick() const;

    int32_t get_counter() const;

    void reset_counter();

    void set_handler(const AsyncInput::encoder_handler_t &handler);

    bool enable(const AsyncInput::encoder_config_t &config);

    void disable();

protected:
    static EncoderCore *encoders[PCNT_UNIT_MAX];
    static bool _attachedInterrupt;
    pcnt_config_t _pcnt_config{};
    AsyncInput::encoder_config_t _cfg;
    uint32_t _last_rotation_time_us{};
    volatile int32_t _counter = 0;
    bool _enabled = false;
    QueueHandle_t _events_queue = nullptr;
    AsyncInput::encoder_handler_t _handler = nullptr;

    void IRAM_ATTR _interrupt();
};

class Encoder final : public EncoderCore {
};

#endif //ASYNCINPUT_ENCODER_H
