#ifndef ASYNCINPUT_TYPES_H
#define ASYNCINPUT_TYPES_H

#include <hal/gpio_types.h>
#include <stdint.h>

#if not defined(ASYNC_INPUT_MAX_EVENTS)
#define ASYNC_INPUT_MAX_EVENTS 30
#endif

#if not defined(ASYNC_INPUT_DOUBLE_CLICK_TIMEOUT_US)
#define ASYNC_INPUT_DOUBLE_CLICK_TIMEOUT_US 300000
#endif

#if not defined(ASYNC_INPUT_CONTACT_BOUNCE_TIMEOUT_US)
#define ASYNC_INPUT_CONTACT_BOUNCE_TIMEOUT_US 500
#endif

#if not defined(ASYNC_INPUT_FAST_ROTATE_ENCODER_TIMEOUT_US)
#define ASYNC_INPUT_FAST_ROTATE_ENCODER_TIMEOUT_US 20000
#endif

#if not defined(ASYNC_INPUT_ENCODER_COUNTER_MAX_VALUE)
#define ASYNC_INPUT_ENCODER_COUNTER_MAX_VALUE 32767
#endif

#if not defined(ASYNC_INPUT_ENCODER_COUNTER_MIN_VALUE)
#define ASYNC_INPUT_ENCODER_COUNTER_MIN_VALUE -32768
#endif



namespace AsyncInput {

    enum button_event_t : uint8_t {
        PRESS = 0, RELEASE, CLICK, DOUBLE_CLICK
    };

    enum encoder_event_t : uint8_t {
        LEFT = 4, RIGHT, FAST_LEFT, FAST_RIGHT
    };

    struct pin_config_t {
        gpio_num_t pin = GPIO_NUM_NC;
        gpio_pull_mode_t pull_mode = GPIO_FLOATING;
    };

    struct button_config_t {
        pin_config_t pin_cfg{};
        uint32_t max_events = ASYNC_INPUT_MAX_EVENTS;
        uint32_t double_click_timeout_us = ASYNC_INPUT_DOUBLE_CLICK_TIMEOUT_US;
        uint16_t contact_bounce_timeout_us = ASYNC_INPUT_CONTACT_BOUNCE_TIMEOUT_US;
    };

    struct encoder_config_t {
        pin_config_t data_pin_cfg{};
        pin_config_t clock_pin_cfg{};
        int16_t min_counter_value = ASYNC_INPUT_ENCODER_COUNTER_MIN_VALUE;
        int16_t max_counter_value = ASYNC_INPUT_ENCODER_COUNTER_MAX_VALUE;
        uint32_t max_events = ASYNC_INPUT_MAX_EVENTS;
        uint32_t fast_rotate_timeout_us = ASYNC_INPUT_FAST_ROTATE_ENCODER_TIMEOUT_US;
        uint16_t contact_bounce_timeout_us = ASYNC_INPUT_CONTACT_BOUNCE_TIMEOUT_US;
    };


    typedef void (*button_handler_t)(button_event_t event, void *button_handler_context);
    typedef void (*encoder_handler_t)(encoder_event_t event, void *encoder_handler_context);
}

#endif //ASYNCINPUT_TYPES_H