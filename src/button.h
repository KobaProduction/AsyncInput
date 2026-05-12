#ifndef ASYNCINPUT_BUTTON_H
#define ASYNCINPUT_BUTTON_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <driver/gpio.h>

#include "types.h"
#include "utils.h"


class ButtonCore {
public:
    void set_handler(const AsyncInput::button_handler_t &handler);

    bool enable(const AsyncInput::button_config_t &config);

    void disable();

    void tick() const;

    bool is_pressed() const;

protected:
    AsyncInput::button_config_t _cfg;
    uint32_t _last_change_time_us = 0;
    uint32_t _last_click_time_us = 0;
    bool _enabled = false;
    bool _pin_level = false;
    QueueHandle_t _evt_queue = nullptr;
    AsyncInput::button_handler_t _handler = nullptr;

    void IRAM_ATTR _interrupt();
};

class Button final : public ButtonCore {
};

#endif //ASYNCINPUT_BUTTON_H
