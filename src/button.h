#ifndef ASYNCINPUT_BUTTON_H
#define ASYNCINPUT_BUTTON_H

#include "types.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"



class Button {
public:
    void set_handler(AsyncInput::button_handler_t handler, void *handler_context = nullptr);
    bool enable(AsyncInput::button_config_t config);
    void disable();
    void tick();
    bool is_pressed();
protected:
    void IRAM_ATTR _interrupt();
    AsyncInput::button_config_t cfg;
    uint32_t _last_change_time_us;
    uint32_t _last_click_time_us;
    bool _pin_level = true;
    QueueHandle_t evt_queue = nullptr;
    void *_handler_context = nullptr;
    AsyncInput::button_handler_t _handler = nullptr;

};

#endif //ASYNCINPUT_BUTTON_H
