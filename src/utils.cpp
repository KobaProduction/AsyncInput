#include "utils.h"

bool prepare_button_pin(AsyncInput::pin_config_t &pin_cfg) {
    static bool isr_installed = false;
    if (not isr_installed) {
        esp_err_t status = gpio_install_isr_service(0);
        if (status == ESP_OK or status == ESP_ERR_INVALID_STATE) isr_installed = true;
        else return false;
    }
    gpio_reset_pin(pin_cfg.pin);
    if (gpio_set_pull_mode(pin_cfg.pin, pin_cfg.pull_mode) not_eq ESP_OK) return false;
    if (gpio_set_direction(pin_cfg.pin, GPIO_MODE_INPUT) not_eq ESP_OK) return false;
    if (gpio_set_intr_type(pin_cfg.pin, GPIO_INTR_ANYEDGE) not_eq ESP_OK) return false;
    return true;
}
