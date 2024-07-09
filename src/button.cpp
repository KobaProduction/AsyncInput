#include "button.h"

void Button::set_handler(AsyncInput::button_handler_t handler, void *handler_context) {
    _handler = handler;
    _handler_context = handler_context;
}

bool Button::enable(AsyncInput::button_config_t config) {
    if (_enabled or config.pin_cfg.pin == GPIO_NUM_NC) return false;
    _cfg = config;

    if (not _cfg.max_events) _cfg.max_events = ASYNC_INPUT_MAX_EVENTS;
    _evt_queue = xQueueCreate(_cfg.max_events, sizeof(AsyncInput::button_event_t));
    if (not _evt_queue) return false;

    if (not _cfg.double_click_timeout_ms) _cfg.double_click_timeout_ms = ASYNC_INPUT_DOUBLE_CLICK_TIMEOUT_MS;
    _cfg.double_click_timeout_ms *= 1000; // convert to us
    _last_change_time_us = 0;
    _last_click_time_us = 0;
    _pin_level = false;

    if (not prepare_pin(_cfg.pin_cfg)) return false;

    esp_err_t status = gpio_isr_handler_add(_cfg.pin_cfg.pin, [] (void *interrupt_context) IRAM_ATTR  {
        Button &self = *reinterpret_cast<Button*>(interrupt_context);
        self._interrupt();
    }, this);
    if (status not_eq ESP_OK) return false;
    _enabled = true;
    return true;
}

void Button::disable() {
    _enabled = false;
    gpio_isr_handler_remove(_cfg.pin_cfg.pin);
    gpio_reset_pin(_cfg.pin_cfg.pin);
    vQueueDelete(_evt_queue);
}

void Button::tick() {
    if (not _enabled) return;
    AsyncInput::button_event_t event;
    if (xQueueReceive(_evt_queue, &event, 0)) {
        if (_handler) _handler(event, _handler_context);
    }
}

bool Button::is_pressed() {
    return _pin_level;
}

void Button::_interrupt() {
    bool last_level = _pin_level;
    _pin_level = gpio_get_level(_cfg.pin_cfg.pin);
    if (_cfg.pin_cfg.inverse_mode) _pin_level = not _pin_level;

    bool is_contact_bounce = esp_timer_get_time() - _last_change_time_us < _cfg.pin_cfg.contact_bounce_timeout_us;
    if (is_contact_bounce or last_level == _pin_level) return;
    _last_change_time_us = esp_timer_get_time();

    AsyncInput::button_event_t event = AsyncInput::RELEASE;
    if (not _pin_level) {
        xQueueSendFromISR(_evt_queue, &event, nullptr);
        return;
    }

    event = AsyncInput::PRESS;
    xQueueSendFromISR(_evt_queue, &event, nullptr);

    bool is_double_click = _last_change_time_us - _last_click_time_us < _cfg.double_click_timeout_ms;
    event = is_double_click ? AsyncInput::DOUBLE_CLICK : AsyncInput::CLICK;
    xQueueSendFromISR(_evt_queue, &event, nullptr);
    _last_click_time_us = _last_change_time_us;
}