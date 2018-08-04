#pragma once

#include <stdbool.h>
#include <string.h>

#include "font.h"

#define PRINT_BUFFER_LINES      64

void Print_Text(const char *text, const Font_t *font);