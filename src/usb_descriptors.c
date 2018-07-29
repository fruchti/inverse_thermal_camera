#include "usb_descriptors.h"

const USB_DeviceDescriptor_t USB_DeviceDescriptor =
{
    .bLength = 18,
    .bDescriptorType = USB_DEVICE_DESCRIPTOR,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0xff,
    .bDeviceSubClass = 0xff,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = 64,
    .idVendor = 0x16c0,
    .idProduct = 0x05dc,
    .bcdDevice = 0x0200,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1
};

const USB_WholeDescriptor_t USB_ConfigurationInterfaceDescriptor =
{
    .configuration = (USB_ConfigurationDescriptor_t)
    {
        .bLength = 9,
        .bDescriptorType = USB_CONFIGURATION_DESCRIPTOR,
        .wTotalLength = sizeof(USB_WholeDescriptor_t),
        .bNumInterfaces = 1,
        .bConfigurationValue = 1,
        .iConfiguration = 0,
        .bmAttributes = 0x80,
        .bMaxPower = 100
    },
    .interface0 = (USB_InterfaceDescriptor_t)
    {
        .bLength = 9,
        .bDescriptorType = USB_INTERFACE_DESCRIPTOR,
        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 1,
        .bInterfaceClass = 0,
        .bInterfaceSubClass = 0,
        .bInterfaceProtocol = 0,
        .iInterface = 0
    },
    .endpoint0out = (USB_EndpointDescriptor_t)
    {
        .bLength = sizeof(USB_EndpointDescriptor_t),
        .bDescriptorType = USB_ENDPOINT_DESCRIPTOR,
        .bEndpointAddress = 1,
        .bmAttributes = 1,
        .wMaxPacketSize = 64,
        .bInterval = 1
    }
};

#define USB_STRING_LANGID                       0x0409
#define USB_STRING_VENDOR                       'F', 'r', 'u', 'c', 'h', 't', \
                                                'i', '\'', 's', 0
#define USB_STRING_PRODUCT                      'e', 'x', 't', 'r', 'e', 'm', \
                                                'e', 'l', 'y', ' ', \
                                                'd', 'u', 'm', 'b', ' ', \
                                                't', 'h', 'e', 'r', 'm', 'a', \
                                                'l', ' ', 'p', 'r', 'i', 'n', \
                                                't', 'e', 'r', 0
#define USB_STRING_SERIAL                       'v', '0', '.', '0', '0', '1', 0

const uint16_t USB_StringDescriptor_LangID[] =
    USB_BUILD_STRING_DESCRIPTOR(USB_STRING_LANGID);
const uint16_t USB_StringDescriptor_Vendor[] =
    USB_BUILD_STRING_DESCRIPTOR(USB_STRING_VENDOR);
const uint16_t USB_StringDescriptor_Product[] =
    USB_BUILD_STRING_DESCRIPTOR(USB_STRING_PRODUCT);
const uint16_t USB_StringDescriptor_Serial[] =
    USB_BUILD_STRING_DESCRIPTOR(USB_STRING_SERIAL);