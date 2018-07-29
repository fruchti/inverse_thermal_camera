#include "usb.h"
#include "usb_descriptors.h"
#include "usb_com.h"

uint8_t USB_DeviceStatus[2] = {0x00, 0x01};
volatile unsigned int USB_ResetCount = 0;
volatile unsigned int USB_Address = 0;

static inline void USB_SetEPR(volatile uint16_t *EPR, uint16_t status)
{
    // Caution: This function does a read-modify-write and is prone to
    // unexpected behaviour when there are transactions going one, because the
    // register contents might change during the funciton's execution. Thus, only
    // use this function in initialisation code!
    volatile uint16_t v = *EPR;
    status ^= v & (USB_EP0R_DTOG_RX | USB_EP0R_STAT_RX |\
        USB_EP0R_DTOG_TX | USB_EP0R_STAT_TX);
    *EPR = status;
}

static inline void USB_SetEPRXStatus(volatile uint16_t *EPR, uint16_t status)
{
    uint16_t v = *EPR;
    v ^= status & USB_EP0R_STAT_RX;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_TYPE | USB_EP0R_EP_KIND | USB_EP0R_CTR_TX | USB_EP0R_EA | USB_EP0R_STAT_RX;
    *EPR = v;
}

static inline void USB_SetEPTXStatus(volatile uint16_t *EPR, uint16_t status)
{
    uint16_t v = *EPR;
    v ^= status & USB_EP0R_STAT_TX;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_TYPE | USB_EP0R_EP_KIND | USB_EP0R_CTR_TX | USB_EP0R_EA | USB_EP0R_STAT_TX;
    *EPR = v;
}

static inline void USB_SetEPType(volatile uint16_t *EPR, uint16_t type)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_KIND | USB_EP0R_CTR_TX | USB_EP0R_EA;
    v |= USB_EP0R_EP_TYPE & type;
    *EPR = v;
}

static inline void USB_SetEPAddress(volatile uint16_t *EPR, uint16_t address)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_TYPE | USB_EP0R_EP_KIND | USB_EP0R_CTR_TX;
    v |= USB_EP0R_EA & address;
    *EPR = v;
}

static inline void USB_SetEPKind(volatile uint16_t *EPR)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_TYPE | USB_EP0R_CTR_TX | USB_EP0R_EA;
    v |= USB_EP0R_EP_KIND;
    *EPR = v;
}

static inline void USB_ClearEPKind(volatile uint16_t *EPR)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_TYPE | USB_EP0R_CTR_TX | USB_EP0R_EA;
    *EPR = v;
}

static inline void USB_ClearEPCTRX(volatile uint16_t *EPR)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_EP_TYPE | USB_EP0R_EP_KIND | USB_EP0R_CTR_TX | USB_EP0R_EA;
    *EPR = v;
}

static inline void USB_ClearEPCTTX(volatile uint16_t *EPR)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_TYPE | USB_EP0R_EP_KIND | USB_EP0R_EA;
    *EPR = v;
}

static inline void USB_Delay(unsigned int delay)
{
    SysTick->LOAD = delay;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while(!((SysTick->CTRL) & SysTick_CTRL_COUNTFLAG_Msk));
	SysTick->CTRL = 0;
}

void USB_Init(void)
{
    // GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USBEN;

    // for(int i = 0; i < 8; i++)
    // {
    //     USB_BTABLE_ENTRIES[i].ADDR_RX = 0;
    //     USB_BTABLE_ENTRIES[i].ADDR_TX = 0;
    //     USB_BTABLE_ENTRIES[i].COUNT_RX = 0;
    //     USB_BTABLE_ENTRIES[i].COUNT_TX = 0;
    // }

    GPIOA->CRH &= ~(GPIO_CRH_CNF11 | GPIO_CRH_MODE11 |\
        GPIO_CRH_CNF12 | GPIO_CRH_MODE12);
    GPIOA->CRH |= GPIO_CRH_MODE11 | GPIO_CRH_MODE12;
    GPIOA->ODR &= ~(GPIO_CRH_MODE11 | GPIO_CRH_MODE12);
    USB_Delay(100000);

    // Enable reset and correct transfer interrupts
    NVIC_EnableIRQ(USB_LP_IRQn);

    // Analog power up
    USB->CNTR = (uint16_t)USB_CNTR_FRES;
    // Minimum delay: 1 µs
    USB_Delay(3000);

    USB->CNTR = (uint16_t)0;
    USB->ISTR = (uint16_t)0;
    USB->CNTR = (uint16_t)(USB_CNTR_RESETM | USB_CNTR_CTRM);

    // Configure USB pins (PA11 and PA12 in AF mode, 50 MHz push-pull)
    GPIOA->CRH |= GPIO_CRH_CNF11_1 | GPIO_CRH_MODE11 |\
        GPIO_CRH_CNF12_1 | GPIO_CRH_MODE12;
}

static inline void USB_HandleReset(void)
{
    // Remove reset flag
    USB->ISTR = (uint16_t)~(USB_ISTR_RESET);
    USB_ResetCount++;

    // Set buffer table origin
    USB->BTABLE = USB_BTABLE_OFFSET;

    // Set buffer table contents for control endpoint 0
    USB_BTABLE_ENTRIES[0].COUNT_RX = USB_EP_RXCOUNT_BL_SIZE | (1 << 10);
    // USB_BTABLE_ENTRIES[0].COUNT_RX = (4 << 10);
    USB_BTABLE_ENTRIES[0].ADDR_RX = 0x40;
    USB_BTABLE_ENTRIES[0].COUNT_TX = 0;
    USB_BTABLE_ENTRIES[0].ADDR_TX = 0x80;

    // Set buffer table contents for control endpoint 1
    USB_BTABLE_ENTRIES[1].COUNT_RX_0 = USB_EP_RXCOUNT_BL_SIZE | (1 << 10);
    USB_BTABLE_ENTRIES[1].ADDR_RX_0 = 0x100;
    USB_BTABLE_ENTRIES[1].COUNT_RX_1 = USB_EP_RXCOUNT_BL_SIZE | (1 << 10);
    USB_BTABLE_ENTRIES[1].ADDR_RX_1 = 0x140;

    // Configure endpoint 0
    USB_SetEPR(&(USB->EP0R), USB_EPR_EP_TYPE_CONTROL |\
        USB_EPR_STAT_TX_NAK | USB_EPR_STAT_RX_VALID);


    // Configure endpoint 1
    USB_SetEPR(&(USB->EP1R), USB_EPR_EP_TYPE_ISO |\
        USB_EPR_STAT_TX_DISABLED | USB_EPR_STAT_RX_VALID |\
        (1 << USB_EP0R_EA_Pos));

    // Enable
    USB->DADDR = USB_DADDR_EF;
}

static inline void USB_PMAToMemory(uint8_t *mem, uint16_t offset, size_t length)
{
    uint8_t *pma = (uint8_t*)(USB_PMA_ADDR + 2 * offset);
    for(unsigned int i = 0; i < length / 2; i++)
    {
        mem[2 * i] = *pma++;
        mem[2 * i + 1] = *pma++;
        pma += 2;
    }
}

static inline void USB_MemoryToPMA(uint16_t offset, const uint8_t *mem, size_t length)
{
    uint16_t *pma = (uint16_t*)(USB_PMA_ADDR + 2 * offset);
    for(unsigned int i = 0; i < length / 2; i++)
    {
        uint16_t tmp = mem[2 * i] | (mem[2 * i + 1] << 8);
        *pma++ = tmp;
        pma++;
    }
}

static inline void USB_HandleIn(void)
{
    if((USB->DADDR & USB_DADDR_ADD) != USB_Address)
    {
        USB->DADDR = USB_Address | USB_DADDR_EF;
    }
    // Ready for next packet
    USB_SetEPRXStatus(&USB->EP0R, USB_EP_RX_VALID);
}

static inline void USB_HandleOut(void)
{
    // Ready for next packet
    USB_SetEPRXStatus(&USB->EP0R, USB_EP_RX_VALID);
}

static inline void USB_HandleSetup(void)
{
    USB_SetupPacket_t sp;
    //memcpy(&sp, (const void*)(USB_PMA_ADDR + USB_BTABLE_ENTRIES[0].ADDR_RX), sizeof(USB_SetupPacket_t));
    USB_PMAToMemory((uint8_t*)&sp, USB_BTABLE_ENTRIES[0].ADDR_RX, sizeof(USB_SetupPacket_t));

    const uint8_t *reply_data = NULL;
    int reply_length = 0;
    uint8_t reply_response = USB_TOKEN_ACK;

    if((sp.bmRequestType & (USB_REQUEST_TYPE | USB_REQUEST_RECIPIENT)) \
        == (USB_REQUEST_TYPE_STANDARD | USB_REQUEST_RECIPIENT_DEVICE))
    {
        switch(sp.bRequest)
        {
            case USB_REQUEST_GET_STATUS:;
                if(sp.wValue == 0 && sp.wIndex == 0 && sp.wLength == 2)
                {
                    reply_length = 2;
                    reply_data = USB_DeviceStatus;
                }
                break;

            case USB_REQUEST_GET_DESCRIPTOR:;
                USB_DescriptorType_t descriptor_type = sp.wValue >> 8;
                int descriptor_index = sp.wValue & 0xff;
                switch(descriptor_type)
                {
                    case USB_DEVICE_DESCRIPTOR:;
                        reply_data = USB_DeviceDescriptor.raw;
                        reply_length = USB_DeviceDescriptor.bLength;
                        break;

                    case USB_CONFIGURATION_DESCRIPTOR:;
                        reply_data = USB_ConfigurationInterfaceDescriptor.raw;
                        if(sp.wLength < USB_ConfigurationInterfaceDescriptor.configuration.wTotalLength)
                        {   
                            reply_length = USB_ConfigurationInterfaceDescriptor.configuration.bLength;
                        }
                        else
                        {
                            reply_length = USB_ConfigurationInterfaceDescriptor.configuration.wTotalLength;
                        }
                        break;

                    case USB_STRING_DESCRIPTOR:;
                        switch(descriptor_index)
                        {
                            case 0:;
                                reply_data = (uint8_t*)USB_StringDescriptor_LangID;
                                reply_length = (uint8_t)*USB_StringDescriptor_LangID;
                                break;
                            case 1:;
                                reply_data = (uint8_t*)USB_StringDescriptor_Vendor;
                                reply_length = (uint8_t)*USB_StringDescriptor_Vendor;
                                break;
                            case 2:;
                                reply_data = (uint8_t*)USB_StringDescriptor_Product;
                                reply_length = (uint8_t)*USB_StringDescriptor_Product;
                                break;
                            case 3:;
                                reply_data = (uint8_t*)USB_StringDescriptor_Serial;
                                reply_length = (uint8_t)*USB_StringDescriptor_Serial;
                                break;
                            default:;
                                __asm__ volatile("bkpt");
                        }
                        break;

                    case USB_INTERFACE_DESCRIPTOR:;
                        reply_data = USB_ConfigurationInterfaceDescriptor.interface0.raw;
                        reply_length = USB_ConfigurationInterfaceDescriptor.interface0.bLength;
                        break;

                    case USB_ENDPOINT_DESCRIPTOR:;
                        // Does not exist yet
                        __asm__ volatile("bkpt");
                        break;
                }
                break;
            
            case USB_REQUEST_SET_ADDRESS:
                USB_Address = sp.wValue & USB_DADDR_ADD;
                reply_response = USB_EP_TX_VALID;
                break;

            case USB_REQUEST_SET_CONFIGURATION:
                reply_response = USB_EP_TX_VALID;
                break;

            default:;
                reply_response = USB_EP_TX_STALL;
                break;
        }
    }
    else
    {
        reply_length = USBCOM_HandleSetupPacket(&sp, &reply_data);
    }

    if(reply_data)
    {
        // Reply with data
        //memcpy((void*)(USB_PMA_ADDR + USB_BTABLE_ENTRIES[0].ADDR_TX), reply_data, reply_length);
        USB_MemoryToPMA(USB_BTABLE_ENTRIES[0].ADDR_TX, reply_data, reply_length);
        USB_BTABLE_ENTRIES[0].COUNT_TX = reply_length;
        USB_SetEPTXStatus(&(USB->EP0R), USB_EP_TX_VALID);
    }
    else
    {
        // Send response
        USB_BTABLE_ENTRIES[0].COUNT_TX = 0;
        USB_SetEPTXStatus(&(USB->EP0R), reply_response);
    }
}

// Interrupt service routine
void USB_LP_IRQHandler(void)
{
    if(USB->ISTR & USB_ISTR_RESET)
    {
        // Reset happened
        USB_HandleReset();
        return;
    }
    uint16_t istr;
    while((istr = USB->ISTR) & (USB_ISTR_CTR))
    {
        if(istr & USB_ISTR_CTR)
        {
            // USB->ISTR = (uint16_t)~USB_ISTR_CTR;
            // Correct transfer
            int ep = istr & USB_ISTR_EP_ID;
            if(ep == 0)
            {
                if(istr & USB_ISTR_DIR)
                {
                    // OUT transfer
                    if(USB->EP0R & USB_EP0R_SETUP)
                    {
                        // Clear CTR_RX
                        USB->EP0R = USB->EP0R & (USB_EP0R_SETUP | USB_EP0R_EP_TYPE |\
                            USB_EP0R_EP_KIND | USB_EP0R_CTR_TX | USB_EP0R_EA);

                        // Setup packed received
                        USB_HandleSetup();
                    }
                    else
                    {
                        // Clear CTR_RX
                        USB->EP0R = USB->EP0R & (USB_EP0R_SETUP | USB_EP0R_EP_TYPE |\
                            USB_EP0R_EP_KIND | USB_EP0R_CTR_TX | USB_EP0R_EA);

                        USB_HandleOut();
                    }
                }
                else
                {
                    // Clear CTR_TX
                    USB->EP0R = USB->EP0R & (USB_EP0R_CTR_RX | USB_EP0R_SETUP |\
                        USB_EP0R_EP_TYPE | USB_EP0R_EP_KIND | USB_EP0R_EA);

                    // IN transfer
                    USB_HandleIn();
                }
            }
            else if(ep == 1)
            {
                USBCOM_HandleISO0OUT();
                // Ready for next packet
                // USB_SetEPRXStatus(&USB->EP1R, USB_EP_RX_VALID);
            }
        }
    }
}