#ifndef ASYNCINPUT_TYPES_H
#define ASYNCINPUT_TYPES_H

#include <hal/gpio_types.h>
#include <stdint.h>

#if not defined(ASYNC_INPUT_MAX_EVENTS)
#define ASYNC_INPUT_MAX_EVENTS 255
#endif

#if not defined(ASYNC_INPUT_DOUBLE_CLICK_TIMEOUT_MS)
#define ASYNC_INPUT_DOUBLE_CLICK_TIMEOUT_MS 300
#endif

#if not defined(ASYNC_INPUT_CONTACT_BOUNCE_TIMEOUT_US)
#define ASYNC_INPUT_CONTACT_BOUNCE_TIMEOUT_US 500
#endif

#if not defined(ASYNC_INPUT_DEFAULT_INVERSE)
#define ASYNC_INPUT_DEFAULT_INVERSE true
#endif

namespace AsyncInput {

    enum button_event_t : uint8_t {
        CLICK, DOUBLE_CLICK, PRESS, RELEASE
    };

    struct pin_config_t {
        gpio_num_t pin = GPIO_NUM_NC;
        gpio_pull_mode_t pull_mode = GPIO_FLOATING;
        uint32_t contact_bounce_timeout_us = ASYNC_INPUT_CONTACT_BOUNCE_TIMEOUT_US;
        bool inverse_mode = ASYNC_INPUT_DEFAULT_INVERSE;
    };

    struct button_config_t {
        pin_config_t pin_cfg{};
        uint32_t max_events = ASYNC_INPUT_MAX_EVENTS;
        uint32_t double_click_timeout_ms = ASYNC_INPUT_DOUBLE_CLICK_TIMEOUT_MS;
    };

    typedef void (*button_handler_t)(button_event_t event, void *button_handler_context);

}

#endif //ASYNCINPUT_TYPES_H