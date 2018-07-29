#pragma once

#include "usb.h"

typedef enum {
    USBCOM_REG_LEDCOUNT,

    USBCOM_N_REGISTERS
} USBCOM_Register_t;

extern unsigned USBCOM_Registers[USBCOM_N_REGISTERS];

uint8_t USBCOM_HandleSetupPacket(USB_SetupPacket_t *sp, const uint8_t **reply_data);
void USBCOM_HandleISO0OUT(void);
