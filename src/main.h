#ifndef MAIN_H_
#define MAIN_H_

#include <stdbool.h>
#include <stdlib.h>
#include "stm32f1xx.h"

#include "pinning.h"
#include "buildid.h"
#include "debug.h"
#include "ov7670.h"
#include "ltp1245.h"
#include "print.h"
#include "font_hannover_messe_serif_26.h"
#include "font_messe_duesseldorf_39.h"
#include "font_arpegius_16.h"
#include "font_arpegius_32.h"
#include "bmp.h"

int main(void);

#endif
