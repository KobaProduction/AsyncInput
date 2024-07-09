#include "utils.h"

bool prepare_pin(AsyncInput::pin_config_t &pin_cfg) {
    esp_err_t status = gpio_install_isr_service(0);
    if (status not_eq ESP_OK and status not_eq ESP_ERR_INVALID_STATE) return false;
    gpio_reset_pin(pin_cfg.pin);

    if (gpio_set_pull_mode(pin_cfg.pin, pin_cfg.pull_mode) not_eq ESP_OK) return false;
    if (gpio_set_direction(pin_cfg.pin, GPIO_MODE_INPUT) not_eq ESP_OK) return false;
    if (gpio_set_intr_type(pin_cfg.pin, GPIO_INTR_ANYEDGE) not_eq ESP_OK) return false;
    if (not pin_cfg.contact_bounce_timeout_us) pin_cfg.contact_bounce_timeout_us = ASYNC_INPUT_CONTACT_BOUNCE_TIMEOUT_US;
    return true;
}
