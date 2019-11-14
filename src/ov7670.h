#pragma once

#include <stdlib.h>
#include "stm32f1xx.h"

#define CAMERA_IMAGE_WIDTH      160
#define CAMERA_IMAGE_HEIGHT     144
#define CAMERA_PIXELS           (CAMERA_IMAGE_WIDTH * CAMERA_IMAGE_HEIGHT)

// Use 2D Floyd-Steinberg dithering. Uncomment to use simple line-wise
// dithering.
#define CAMERA_USE_2D_DITHERING

// Use a primitive autoexposure by shifting each frames luminosity based on the
// ratio of white to black pixels in the previous frames
// #define CAMERA_USE_EXPOSURE_CORRECTION

extern volatile int Camera_Captured;

void Camera_Init(void);