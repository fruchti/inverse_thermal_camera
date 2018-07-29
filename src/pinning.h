#pragma once

// Port A
#define PIN_STEPPER_AP          0   // PA0  - Stepper phase A positive
#define PIN_STEPPER_BP          1   // PA1  - Stepper phase B positive
#define PIN_HEAD                2   // PA2  - Head up sensor
#define PIN_PAPER               3   // PA3  - Paper detect
#define PIN_STEPPER_AM          4   // PA4  - Stepper phase A negative
#define PIN_STEPPER_BM          5   // PA5  - Stepper phase B negative

#define PIN_USB_DM              11  // PA11 - USB D-
#define PIN_USB_DP              12  // PA12 - USB D+

// Port B
#define PIN_THERMISTOR          0   // PB0  - Thermistor
#define PIN_DST1                10  // PB10 - Head drive signal 1 (TIM2_CH3)
#define PIN_DST2                11  // PB11 - Head drive signal 2 (TIM2_CH4)
#define PIN_SCK                 13  // PB13 - Thermal printer SCK (SPI2)
#define PIN_LATCH               14  // PB14 - Thermal printer data latch
#define PIN_DIN                 15  // PB15 - Thermal printer MOSI (SPI2)

// Port C
#define PIN_LED                 13  // PC13 - Status LED
