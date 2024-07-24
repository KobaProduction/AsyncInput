#include <Arduino.h>
#include <AsyncInput.h>

#define ENCODER_CLK_PIN 4
#define ENCODER_DT_PIN 16
#define ENCODER_BTN_PIN 17

Button btn;
Encoder enc;

void setup() {
    Serial.begin(1000000);
    Serial.println();
    Serial.println("Start test");

    AsyncInput::button_config_t btn_cfg;
    btn_cfg.pin_cfg.pin = gpio_num_t(ENCODER_BTN_PIN);
    btn.set_handler([] (AsyncInput::button_event_t event, void *ctx) {
        switch (event) {
            case AsyncInput::CLICK:
                Serial.println("CLICK");
                break;
            case AsyncInput::DOUBLE_CLICK:
                Serial.println("DOUBLE_CLICK");
                break;
            case AsyncInput::PRESS:
                Serial.println("PRESS");
                break;
            case AsyncInput::RELEASE:
                Serial.println("RELEASE");
                break;
        }
    });
    btn.enable(btn_cfg);

    AsyncInput::encoder_config_t enc_cfg;
    enc_cfg.clock_pin_cfg.pin = gpio_num_t(ENCODER_CLK_PIN);
    enc_cfg.data_pin_cfg.pin = gpio_num_t(ENCODER_DT_PIN);
    enc_cfg.max_events = 255;
    enc.set_handler([] (AsyncInput::encoder_event_t event, void *ctx) {
        Encoder &enc = *reinterpret_cast<Encoder*>(ctx);
        static uint32_t last_time = esp_timer_get_time();
        if (esp_timer_get_time() - last_time > 1000000) {
            Serial.println();
            last_time = esp_timer_get_time();
        }
        Serial.print(enc.get_counter());
        Serial.print(" ");
        switch (event) {
            case AsyncInput::LEFT:
                Serial.println("LEFT");
                break;
            case AsyncInput::RIGHT:
                Serial.println("RIGHT");
                break;
            case AsyncInput::FAST_LEFT:
                Serial.println("FAST_LEFT");
                break;
            case AsyncInput::FAST_RIGHT:
                Serial.println("FAST_RIGHT");
                break;
        }
    }, &enc);

    if (enc.enable(enc_cfg)) {
        Serial.println("ENC started");
    }

}

void loop() {
    btn.tick();
    enc.tick();
}