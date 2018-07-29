#pragma once

#include <string.h>
#include "stm32f1xx.h"

#include "pinning.h"
#include "debug.h"

#define USB_TOKEN_OUT                           0b0001
#define USB_TOKEN_IN                            0x1001
#define USB_TOKEN_SOF                           0b0101
#define USB_TOKEN_SETUP                         0b1101
#define USB_TOKEN_DATA0                         0b0011
#define USB_TOKEN_DATA1                         0b1011
#define USB_TOKEN_DATA2                         0b0111
#define USB_TOKEN_ACK                           0b0010
#define USB_TOKEN_NAK                           0b1010
#define USB_TOKEN_STALL                         0b1110
#define USB_TOKEN_NYET                          0b0110
#define USB_TOKEN_PRE                           0b1100
#define USB_TOKEN_ERR                           0b1100
#define USB_TOKEN_SPLIT                         0b1000
#define USB_TOKEN_PING                          0b0100

#define USB_REQUEST_DIRECTION                   (1 << 7)
#define USB_REQUEST_DIRECTION_OUT               0
#define USB_REQUEST_DIRECTION_IN                (1 << 7)
#define USB_REQUEST_TYPE                        (0x3 << 5)
#define USB_REQUEST_TYPE_STANDARD               0
#define USB_REQUEST_TYPE_CLASS                  (1 << 5)
#define UBS_REQUEST_TYPE_VENDOR                 (2 << 5)
#define UBS_REQUEST_TYPE_RESERVED               (3 << 5)
#define USB_REQUEST_RECIPIENT                   0x1f
#define USB_REQUEST_RECIPIENT_DEVICE            0
#define USB_REQUEST_RECIPIENT_INTERFACE         1
#define USB_REQUEST_RECIPIENT_ENDPOINT          2
#define USB_REQUEST_RECIPIENT_OTHER             3

#define USB_REQUEST_CLEAR_FEATURE               1   // wValue = <feature>, wIndex = 0|<interface>|<endpoint>, wLength = 0
#define USB_REQUEST_GET_CONFIGURATION           8   // wValue = 0, wIndex = 0, wLength = 1
#define USB_REQUEST_GET_DESCRIPTOR              6   // wValue = <descriptor type>:<descriptor index>, wIndex = 0|<language>, wLength = <descriptor length>
#define USB_REQUEST_GET_INTERFACE               10  // wValue = 0, wIndex = <interface>, wLength = 1
#define USB_REQUEST_GET_STATUS                  0   // wValue = 0, wIndex = 0|<interface>|<endpoint>, wLength = 2
#define USB_REQUEST_SET_ADDRESS                 5   // wValue = <address>, wIndex = 0, wLength = 0
#define USB_REQUEST_SET_CONFIGURATION           9   // wValue = <configuration value>, wIndex = 0, wLength = 0
#define USB_REQUEST_SET_DESCRIPTOR              7   // wValue = <descriptor type>:<descriptor index>, wIndex = 0|<language>, wLength = <descriptor length>
#define USB_REQUEST_SET_FEATURE                 3   // wValue = <feature selector>, wIndex = 0|<interface>|<endpoint>, wLength = 0
#define USB_REQUEST_SET_INTERFACE               11  // wValue = <alternate setting>, wIndex = <interface>, wLength = 0
#define USB_REQUEST_SYNCH_FRAME                 12  // wValue = 0, wIndex = <endpoint>, wLength = 2

#define USB_EPR_STAT_TX_DISABLED                0x00
#define USB_EPR_STAT_TX_STALL                   USB_EP0R_STAT_TX_0
#define USB_EPR_STAT_TX_NAK                     USB_EP0R_STAT_TX_1
#define USB_EPR_STAT_TX_VALID                   (USB_EP0R_STAT_TX_0 | USB_EP0R_STAT_TX_1)

#define USB_EPR_STAT_RX_DISABLED                0x00
#define USB_EPR_STAT_RX_STALL                   USB_EP0R_STAT_RX_0
#define USB_EPR_STAT_RX_NAK                     USB_EP0R_STAT_RX_1
#define USB_EPR_STAT_RX_VALID                   (USB_EP0R_STAT_RX_0 | USB_EP0R_STAT_RX_1)

#define USB_EPR_EP_TYPE_BULK                    0x00
#define USB_EPR_EP_TYPE_CONTROL                 USB_EP0R_EP_TYPE_0
#define USB_EPR_EP_TYPE_ISO                     USB_EP0R_EP_TYPE_1
#define USB_EPR_EP_TYPE_INTERRUPT               (USB_EP0R_EP_TYPE_0 | USB_EP0R_EP_TYPE_1)

#define USB_PMA_ADDR                            0x40006000UL
#define USB_BTABLE_OFFSET                       0x00
#define USB_EP_RXCOUNT_BL_SIZE                  (1 << 15)

typedef struct
{
    volatile union
    {
        volatile uint16_t ADDR_TX;
        volatile uint16_t ADDR_RX_0;
        volatile uint16_t ADDR_TX_0;
    };
    volatile uint16_t rsvd1;
    volatile union
    {
        volatile uint16_t COUNT_TX;
        volatile uint16_t COUNT_RX_0;
        volatile uint16_t COUNT_TX_0;
    };
    volatile uint16_t rsvd2;
    volatile union
    {
        volatile uint16_t ADDR_RX;
        volatile uint16_t ADDR_RX_1;
        volatile uint16_t ADDR_TX_1;
    };
    volatile uint16_t rsvd3;
    volatile union
    {
        volatile uint16_t COUNT_RX;
        volatile uint16_t COUNT_RX_1;
        volatile uint16_t COUNT_TX_1;
    };
    volatile uint16_t rsvd4;
} __attribute__((packed, aligned(4))) USB_BufferTableEntry_t;

typedef struct
{
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __attribute__((packed, aligned(2))) USB_SetupPacket_t;

#define USB_BTABLE_ENTRIES                      ((USB_BufferTableEntry_t*)(USB_PMA_ADDR + USB_BTABLE_OFFSET))

void USB_Init(void);
