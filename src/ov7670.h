#pragma once

#include <stdlib.h>
#include "stm32f1xx.h"

#define CAMERA_IMAGE_WIDTH      160
#define CAMERA_IMAGE_HEIGHT     144

extern volatile int Camera_Captured;

void Camera_Init(void);