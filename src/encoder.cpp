#include "encoder.h"

Encoder *Encoder::encoders[PCNT_UNIT_MAX] = {nullptr};
bool Encoder::_attachedInterrupt = false;
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

bool Encoder::enable(AsyncInput::encoder_config_t config) {
    if (_enabled or config.data_pin_cfg.pin == GPIO_NUM_NC or config.clock_pin_cfg.pin == GPIO_NUM_NC) return false;

    if (not _cfg.max_events) _cfg.max_events = ASYNC_INPUT_MAX_EVENTS;
    _events_queue = xQueueCreate(_cfg.max_events, sizeof(AsyncInput::encoder_event_t));
    if (not _events_queue) return false;

    for (int i = 0; i < PCNT_UNIT_MAX; i++) {
        if (not Encoder::encoders[i]) {
            encoders[i] = this;
            _pcnt_config.unit = static_cast<pcnt_unit_t>(i);
            break;
        }
        if (i == PCNT_UNIT_MAX - 1) return false; // max encoders
    }

    _cfg = config;

    if (gpio_set_pull_mode(_cfg.data_pin_cfg.pin, _cfg.data_pin_cfg.pull_mode) not_eq ESP_OK) return false;
    if (gpio_set_direction(_cfg.data_pin_cfg.pin, GPIO_MODE_INPUT) not_eq ESP_OK) return false;
    if (gpio_set_pull_mode(_cfg.clock_pin_cfg.pin, _cfg.clock_pin_cfg.pull_mode) not_eq ESP_OK) return false;
    if (gpio_set_direction(_cfg.clock_pin_cfg.pin, GPIO_MODE_INPUT) not_eq ESP_OK) return false;

    _pcnt_config.pulse_gpio_num = _cfg.data_pin_cfg.pin;
    _pcnt_config.ctrl_gpio_num = _cfg.clock_pin_cfg.pin;
    _pcnt_config.channel = PCNT_CHANNEL_0;
    _pcnt_config.pos_mode = PCNT_COUNT_DIS;
    _pcnt_config.neg_mode = PCNT_COUNT_INC;
    _pcnt_config.lctrl_mode = PCNT_MODE_KEEP;    // Rising A on HIGH B = CW Step
    _pcnt_config.hctrl_mode = PCNT_MODE_REVERSE; // Rising A on LOW B = CCW Step
    _pcnt_config.counter_h_lim = _cfg.max_counter_value;
    _pcnt_config.counter_l_lim = _cfg.min_counter_value;

    pcnt_unit_config(&_pcnt_config);

    if (_cfg.contact_bounce_timeout_us > 1023) _cfg.contact_bounce_timeout_us = 1023;
    pcnt_set_filter_value(_pcnt_config.unit, _cfg.contact_bounce_timeout_us);
    pcnt_filter_enable(_pcnt_config.unit);

    if (not _attachedInterrupt) {
        if (pcnt_isr_service_install(0) not_eq ESP_OK) return false;
        _attachedInterrupt = true;
    }

    static IRAM_ATTR auto isr_pcnt_handler =  [] (void *interrupt_context)  {
        reinterpret_cast<Encoder*>(interrupt_context)->_interrupt();
    };

    if (pcnt_isr_handler_add(_pcnt_config.unit, isr_pcnt_handler, this) not_eq ESP_OK) {
        return false;
    }

    pcnt_set_event_value(_pcnt_config.unit, PCNT_EVT_THRES_0, -1);
    pcnt_set_event_value(_pcnt_config.unit, PCNT_EVT_THRES_1, 1);
    pcnt_event_enable(_pcnt_config.unit, PCNT_EVT_THRES_0);
    pcnt_event_enable(_pcnt_config.unit, PCNT_EVT_THRES_1);
    pcnt_counter_clear(_pcnt_config.unit);
    pcnt_intr_enable(_pcnt_config.unit);

    _last_rotation_time_us = 0;
    _enabled = true;
    return true;
}

void Encoder::disable() {
    _enabled = false;
    pcnt_intr_disable(_pcnt_config.unit);
    bool need_disable_isr = true;
    Encoder::encoders[_pcnt_config.unit] = nullptr;
    for (auto &encoder : Encoder::encoders) {
        if (not encoder) continue;
        need_disable_isr = false;
        break;
    }
    if (need_disable_isr) {
        pcnt_isr_service_uninstall();
        _attachedInterrupt = false;
    }
    pcnt_isr_handler_remove(this->_pcnt_config.unit);
    pcnt_filter_disable(_pcnt_config.unit);
    gpio_reset_pin(_cfg.data_pin_cfg.pin);
    gpio_reset_pin(_cfg.clock_pin_cfg.pin);
    vQueueDelete(_events_queue);
}

void Encoder::_interrupt() {
    AsyncInput::encoder_event_t event;
    static int16_t counter;
    portENTER_CRITICAL_SAFE(&spinlock);
    pcnt_get_counter_value(_pcnt_config.unit, &counter);
    pcnt_counter_clear(_pcnt_config.unit);
    portEXIT_CRITICAL_SAFE(&spinlock);
    if (not counter) return;
    bool is_fast_rotation = ((esp_timer_get_time() - _last_rotation_time_us) / abs(counter)) < _cfg.fast_rotate_timeout_us;
    _last_rotation_time_us = esp_timer_get_time();
    _counter += is_fast_rotation ? counter * 10: counter;
    if (counter > 0) event = is_fast_rotation ? AsyncInput::FAST_LEFT : AsyncInput::LEFT;
    else event = is_fast_rotation ? AsyncInput::FAST_RIGHT : AsyncInput::RIGHT;
    xQueueSendFromISR(_events_queue, &event, nullptr);
}

void Encoder::tick() {
    if (not _enabled) return;
    AsyncInput::encoder_event_t event;
    if (xQueueReceive(_events_queue, &event, 0)) {
        if (_handler) _handler(event, _handler_context);
    }
}

int32_t Encoder::get_counter() const {
    return _counter;
}

void Encoder::reset_counter() {
    _counter = 0;
}

void Encoder::set_handler(AsyncInput::encoder_handler_t handler, void *handler_context) {
    _handler = handler;
    _handler_context = handler_context;
}