#pragma once

#include <stdbool.h>

#include "stm32f1xx.h"

typedef int (*LTP1245_DataProvider_t)(uint8_t *buffer);

#define LTP1245_MAX_DRIVE_FREQ  473     // In Hz
#define LTP1245_BUFFER_LINES    64

#define LTP1245_TH_REXT         10      // In kΩ

void LTP1245_Init(void);
