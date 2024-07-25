#include <Arduino.h>
#include <AsyncInput.h>

#define ENCODER_CLK_PIN 4
#define ENCODER_DT_PIN 16
#define ENCODER_BTN_PIN 17

Encoder enc;

void setup() {
    Serial.begin(115200);
    Serial.println();

    AsyncInput::encoder_config_t enc_cfg;
    enc_cfg.clock_pin_cfg.pin = gpio_num_t(ENCODER_CLK_PIN);
    enc_cfg.data_pin_cfg.pin = gpio_num_t(ENCODER_DT_PIN);
    enc_cfg.max_events = 50;

    enc.set_handler([] (AsyncInput::encoder_event_t event, void *ctx) {
        Encoder &enc = *reinterpret_cast<Encoder*>(ctx);
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
    enc.tick();
}