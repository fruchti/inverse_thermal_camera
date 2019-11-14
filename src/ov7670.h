#pragma once

#include <stdlib.h>
#include "stm32f1xx.h"

#define CAMERA_IMAGE_WIDTH      160
#define CAMERA_IMAGE_HEIGHT     144
#define CAMERA_PIXELS           (CAMERA_IMAGE_WIDTH * CAMERA_IMAGE_HEIGHT)

// Maximum and minimum share of black pixels for an image to be accepted
#define CAMERA_EXPOSURE_LIMIT   0.4
#define CAMERA_EXPOSURE_LOW     (0.5 - CAMERA_EXPOSURE_LIMIT)
#define CAMERA_EXPOSURE_HIGH    (0.5 + CAMERA_EXPOSURE_LIMIT)

// Use 2D Floyd-Steinberg dithering. Uncomment to use simple line-wise
// dithering.
#define CAMERA_USE_2D_DITHERING

extern volatile int Camera_Captured;

void Camera_Init(void);