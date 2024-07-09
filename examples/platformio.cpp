#include <Arduino.h>
#include <AsyncInput.h>

Button btn;

#define ENCODER_CLK_PIN 39
#define ENCODER_DT_PIN 36
#define ENCODER_BTN_PIN 34

void setup() {
    Serial.begin(1000000);
    Serial.println();
    Serial.println("Start test");
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
    AsyncInput::button_config_t cfg;
    cfg.pin = gpio_num_t(ENCODER_BTN_PIN);
    btn.enable(cfg);
}



void loop() {
    btn.tick();
//    Serial.println(gpio_get_level(gpio_num_t(ENCODER_BTN_PIN)));
}