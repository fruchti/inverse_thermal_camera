#pragma once

// Port A
#define PIN_CAMERA_DB0          0   // PA0  - Camera data line 0
#define PIN_CAMERA_DB1          1   // PA1  - Camera data line 1
#define PIN_CAMERA_DB2          2   // PA2  - Camera data line 2
#define PIN_CAMERA_DB3          3   // PA3  - Camera data line 3
#define PIN_CAMERA_DB4          4   // PA4  - Camera data line 4
#define PIN_CAMERA_DB5          5   // PA5  - Camera data line 5
#define PIN_CAMERA_DB6          6   // PA6  - Camera data line 6
#define PIN_CAMERA_DB7          7   // PA7  - Camera data line 7
#define PIN_CAMERA_MCLK         8   // PA8  - Camera main clock (MCO/TIM1_CH1)
#define PIN_CAMERA_VSYNC        9   // PA9  - Camera VSYNC (TIM1_CH2)
#define PIN_STEPPER_AP          10  // PA10 - Stepper phase A positive
#define PIN_STEPPER_BP          11  // PA11 - Stepper phase B positive
#define PIN_STEPPER_AM          12  // PA12 - Stepper phase A negative
#define PIN_STEPPER_BM          15  // PA15 - Stepper phase B negative

// Port B
#define PIN_THERMISTOR          0   // PB0  - Thermistor (ADC12_IN8)
#define PIN_VBATT               1   // PB1  - Battery monitor (ADC12_IN9)
#define PIN_SUPPLY              2   // PB2  - Voltage regulator enable
#define PIN_PAPER               3   // PB3  - Paper detect
#define PIN_CAMERA_PCLK         4   // PB4  - Camera pixel clock (TIM3_CH1)
#define PIN_CAMERA_HSYNC        5   // PB5  - Camera VSYNC (TIM3_CH2)
#define PIN_HEAD                6   // PB6  - Head up sensor
#define PIN_PAPER_CUT           7   // PB7  - Paper cutter servo (TIM4_CH2)
#define PIN_CAMERA_SCL          8   // PB8  - Camera control I2C (I2C1_SCL)
#define PIN_CAMERA_SDA          9   // PB9  - Camera control I2C (I2C1_SDA)
#define PIN_DST1                10  // PB10 - Head drive signal 1 (TIM2_CH3)
#define PIN_DST2                11  // PB11 - Head drive signal 2 (TIM2_CH4)
#define PIN_CAMERA_RESET        12  // PB12 - Camera reset (active low)
#define PIN_SCK                 13  // PB13 - Thermal printer SCK (SPI2)
#define PIN_LATCH               14  // PB14 - Thermal printer data latch
#define PIN_DIN                 15  // PB15 - Thermal printer MOSI (SPI2)

// Port C
#define PIN_LED                 13  // PC13 - Status LED
