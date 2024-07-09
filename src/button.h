#ifndef ASYNCINPUT_BUTTON_H
#define ASYNCINPUT_BUTTON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <driver/gpio.h>

#include "types.h"
#include "utils.h"


class Button {
public:
    void set_handler(AsyncInput::button_handler_t handler, void *handler_context = nullptr);
    bool enable(AsyncInput::button_config_t config);
    void disable();
    void tick();
    bool is_pressed();
protected:
    AsyncInput::button_config_t _cfg;
    uint32_t _last_change_time_us;
    uint32_t _last_click_time_us;
    bool _enabled = false;
    bool _pin_level;
    QueueHandle_t _evt_queue = nullptr;
    void *_handler_context = nullptr;
    AsyncInput::button_handler_t _handler = nullptr;
    void IRAM_ATTR _interrupt();
};

#endif //ASYNCINPUT_BUTTON_H