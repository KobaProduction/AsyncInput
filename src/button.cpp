#include "button.h"


void Button::set_handler(AsyncInput::button_handler_t handler, void *handler_context) {
    _handler = handler;
    _handler_context = handler_context;
}

#define GPIO_INPUT_IO_0     4
#define GPIO_INPUT_IO_1     5
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
#define ESP_INTR_FLAG_DEFAULT 0

#include "Arduino.h"

bool Button::enable(AsyncInput::button_config_t config) {
    if (config.pin_cfg.pin == GPIO_NUM_NC or cfg.pin_cfg.pin not_eq GPIO_NUM_NC) return false;
    cfg = config;

    esp_err_t status = gpio_install_isr_service(0);
    if (status not_eq ESP_OK and status not_eq ESP_ERR_INVALID_STATE) return false;
    gpio_reset_pin(cfg.pin_cfg.pin);

    if (not cfg.max_events) cfg.max_events = ASYNC_INPUT_MAX_EVENTS;
    evt_queue = xQueueCreate(cfg.max_events, sizeof(AsyncInput::button_event_t));
    if (not evt_queue) return false;

    if (gpio_set_pull_mode(cfg.pin_cfg.pin, cfg.pin_cfg.pull_mode) not_eq ESP_OK) return false;
    if (gpio_set_direction(cfg.pin_cfg.pin, GPIO_MODE_INPUT) not_eq ESP_OK) return false;
    if (gpio_set_intr_type(cfg.pin_cfg.pin, GPIO_INTR_ANYEDGE) not_eq ESP_OK) return false;

    if (not cfg.pin_cfg.contact_bounce_timeout_us) cfg.pin_cfg.contact_bounce_timeout_us = ASYNC_INPUT_CONTACT_BOUNCE_TIMEOUT_US;
    if (not cfg.double_click_timeout_ms) cfg.double_click_timeout_ms = ASYNC_INPUT_DOUBLE_CLICK_TIMEOUT_MS;

    cfg.double_click_timeout_ms *= 1000; // convert to us

    _last_change_time_us = 0;
    _last_click_time_us = 0;
    _pin_level = false;

    status = gpio_isr_handler_add(cfg.pin_cfg.pin, [] (void *interrupt_context) IRAM_ATTR  {
        Button &self = *reinterpret_cast<Button*>(interrupt_context);
        self._interrupt();
    }, this);
    if (status not_eq ESP_OK) return false;
    return true;
}

void Button::disable() {
    gpio_isr_handler_remove(cfg.pin_cfg.pin);
    gpio_reset_pin(cfg.pin_cfg.pin);
    vQueueDelete(evt_queue);
    cfg.pin_cfg.pin = GPIO_NUM_NC;
}

void Button::_interrupt() {
    bool last_level = _pin_level;
    _pin_level = gpio_get_level(cfg.pin_cfg.pin);
    if (cfg.pin_cfg.inverse_mode) _pin_level = not _pin_level;

    bool is_contact_bounce = esp_timer_get_time() - _last_change_time_us < cfg.pin_cfg.contact_bounce_timeout_us;
    if (is_contact_bounce or last_level == _pin_level) return;
    _last_change_time_us = esp_timer_get_time();

    AsyncInput::button_event_t event = AsyncInput::RELEASE;
    if (not _pin_level) {
        xQueueSendFromISR(evt_queue, &event, nullptr);
        return;
    }

    event = AsyncInput::PRESS;
    xQueueSendFromISR(evt_queue, &event, nullptr);

    bool is_double_click = _last_change_time_us - _last_click_time_us < cfg.double_click_timeout_ms;
    event = is_double_click ? AsyncInput::DOUBLE_CLICK : AsyncInput::CLICK;
    xQueueSendFromISR(evt_queue, &event, nullptr);
    _last_click_time_us = _last_change_time_us;
}

void Button::tick() {
    AsyncInput::button_event_t event;
    if (xQueueReceive(evt_queue, &event, 0)) {
        if (_handler) _handler(event, _handler_context);
    }
}

bool Button::is_pressed() {
    return _pin_level;
}
