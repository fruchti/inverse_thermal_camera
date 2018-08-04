#pragma once

#include <stdbool.h>
#include <string.h>

#include "stm32f1xx.h"

typedef enum
{
    LTP1245_OK,
    LTP1245_NO_PAPER,
    LTP1245_HEAD_UP
} LTP1245_Result_t;

#define LTP1245_MAX_DRIVE_FREQ  473     // In Hz
#define LTP1245_BUFFER_LINES    64
#define LTP1245_LINEWIDTH       384     // In pixels

#define LTP1245_TH_REXT         10      // In kΩ

void LTP1245_Init(void);
LTP1245_Result_t LTP1245_FeedPaper(int lines);
LTP1245_Result_t LTP1245_Print(uint8_t *data, int lines);
