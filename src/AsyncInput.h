#ifndef ASYNCINPUT_ASYNCINPUT_H
#define ASYNCINPUT_ASYNCINPUT_H

#if not defined(CONFIG_IDF_TARGET_ESP32) or not CONFIG_IDF_TARGET_ESP32
#error THE LIBRARY IS CURRENTLY ONLY AVAILABLE ON ESP32
#endif

#if not defined(ASYNCINPUT_BUTTON_H)
#include "button.h"
#endif

#if not defined(ASYNCINPUT_ENCODER_H)
#include "encoder.h"
#endif

#endif //ASYNCINPUT_ASYNCINPUT_H
