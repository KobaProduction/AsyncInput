#include "button.h"

void ButtonCore::set_handler(const AsyncInput::button_handler_t &handler) {
    _handler = handler;
}

bool ButtonCore::enable(const AsyncInput::button_config_t &config) {
    if (_enabled or config.pin_cfg.pin == GPIO_NUM_NC) return false;
    _cfg = config;

    if (not _cfg.max_events) _cfg.max_events = ASYNC_INPUT_MAX_EVENTS;
    _evt_queue = xQueueCreate(_cfg.max_events, sizeof(AsyncInput::button_event_t));
    if (not _evt_queue) return false;

    if (not _cfg.double_click_timeout_us) _cfg.double_click_timeout_us = ASYNC_INPUT_DOUBLE_CLICK_TIMEOUT_US;
    if (not _cfg.contact_bounce_timeout_us) _cfg.contact_bounce_timeout_us = ASYNC_INPUT_CONTACT_BOUNCE_TIMEOUT_US;
    _last_change_time_us = 0;
    _last_click_time_us = 0;
    _pin_level = false;

    if (not prepare_button_pin(_cfg.pin_cfg)) return false;

    static IRAM_ATTR auto isr_button_handler = [](void *interrupt_context) {
        static_cast<ButtonCore *>(interrupt_context)->_interrupt();
    };

    if (gpio_isr_handler_add(_cfg.pin_cfg.pin, isr_button_handler, this) not_eq ESP_OK) return false;
    _enabled = true;
    return true;
}

void ButtonCore::disable() {
    _enabled = false;
    gpio_isr_handler_remove(_cfg.pin_cfg.pin);
    gpio_reset_pin(_cfg.pin_cfg.pin);
    vQueueDelete(_evt_queue);
}

void ButtonCore::tick() const {
    if (not _enabled) return;
    AsyncInput::button_event_t event;
    if (xQueueReceive(_evt_queue, &event, 0)) {
        if (_handler) _handler(event);
    }
}

bool ButtonCore::is_pressed() const {
    return _cfg.pin_cfg.inverse_mode ? not _pin_level : _pin_level;
}

void ButtonCore::_interrupt() {
    const bool last_level = _pin_level;
    _pin_level = gpio_get_level(_cfg.pin_cfg.pin);

    const bool is_contact_bounce = esp_timer_get_time() - _last_change_time_us < _cfg.contact_bounce_timeout_us;
    if (is_contact_bounce or last_level == _pin_level) return;
    _last_change_time_us = esp_timer_get_time();

    AsyncInput::button_event_t event = AsyncInput::RELEASE;
    if (not is_pressed()) {
        xQueueSendFromISR(_evt_queue, &event, nullptr);
        return;
    }

    event = AsyncInput::PRESS;
    xQueueSendFromISR(_evt_queue, &event, nullptr);

    const bool is_double_click = _last_change_time_us - _last_click_time_us < _cfg.double_click_timeout_us;
    event = is_double_click ? AsyncInput::DOUBLE_CLICK : AsyncInput::CLICK;
    xQueueSendFromISR(_evt_queue, &event, nullptr);
    _last_click_time_us = _last_change_time_us;
}
