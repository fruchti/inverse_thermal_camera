#pragma once

#include <stdlib.h>
#include "stm32f1xx.h"

#define CAMERA_IMAGE_WIDTH      160
#define CAMERA_IMAGE_HEIGHT     144

// Use 2D Floyd-Steinberg dithering. Uncomment to use simple line-wise
// dithering.
#define CAMERA_USE_2D_DITHERING

extern volatile int Camera_Captured;

void Camera_Init(void);