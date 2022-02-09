#pragma once

#define POS_CORRECTION -9

#define NUM_DEG 360
#define NUM_LEDS 96
#define LED_RATIO NUM_DEG / NUM_LEDS

#define OUTPUT_PIN 27

#define DEFAULT_BRIGHTNESS 96

#define SYMBOL_LED_WIDTH 26
#define SYMBOL_WIDTH 30
#define SYMBOL_OFFSET 23

#define MILLIS_IN_MINUTE (60 * 1000)
#define MICROS_IN_MINUTE (MILLIS_IN_MINUTE * 1000)

#define SEC_TO_MICRO 1000000

#define MAX_ANIM_LENGHT (5 * 60 * SEC_TO_MICRO)
