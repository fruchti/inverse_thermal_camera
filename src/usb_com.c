#include "usb_com.h"

unsigned USBCOM_Registers[USBCOM_N_REGISTERS];

uint8_t USBCOM_HandleSetupPacket(USB_SetupPacket_t *sp, const uint8_t **reply_data)
{
    uint8_t reply_length = 0;

    if(sp->bmRequestType & USB_REQUEST_DIRECTION_IN)
    {
        // Read register
        if(sp->bRequest < USBCOM_N_REGISTERS)
        {
            *reply_data = (uint8_t*)&USBCOM_Registers[sp->bRequest];
            reply_length = sp->wLength;
        }
    }
    else
    {
        // Write register
        if(sp->bRequest < USBCOM_N_REGISTERS)
        {
            USBCOM_Registers[sp->bRequest] = ((uint32_t)(sp->wIndex) << 16) | sp->wValue;
        }
    }

    return reply_length;
}

void USBCOM_HandleISO0OUT(void)
{
    __asm__("bkpt");
}