#pragma once

#include <stdint.h>

typedef union
{
    struct
    {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint16_t bcdUSB;
        uint8_t bDeviceClass;
        uint8_t bDeviceSubClass;
        uint8_t bDeviceProtocol;
        uint8_t bMaxPacketSize0;
        uint16_t idVendor;
        uint16_t idProduct;
        uint16_t bcdDevice;
        uint8_t iManufacturer;
        uint8_t iProduct;
        uint8_t iSerialNumber;
        uint8_t bNumConfigurations;
    };
    uint8_t raw[18];
} __attribute__((packed)) USB_DeviceDescriptor_t;

typedef union
{
    struct
    {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint16_t wTotalLength;
        uint8_t bNumInterfaces;
        uint8_t bConfigurationValue;
        uint8_t iConfiguration;
        uint8_t bmAttributes;
        uint8_t bMaxPower;        
    } __attribute__((packed, aligned(1)));
    uint8_t raw[9];
} __attribute__((packed, aligned(1))) USB_ConfigurationDescriptor_t;

typedef union
{
    struct
    {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint8_t bInterfaceNumber;
        uint8_t bAlternateSetting;
        uint8_t bNumEndpoints;
        uint8_t bInterfaceClass;
        uint8_t bInterfaceSubClass;
        uint8_t bInterfaceProtocol;
        uint8_t iInterface;        
    } __attribute__((packed, aligned(1)));
    uint8_t raw[9];
} __attribute__((packed, aligned(1))) USB_InterfaceDescriptor_t;

typedef union
{
    struct
    {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint8_t bEndpointAddress;
        uint8_t bmAttributes;
        uint16_t wMaxPacketSize;
        uint8_t bInterval;        
    };
    uint8_t raw[7];
} __attribute__((packed)) USB_EndpointDescriptor_t;

typedef union
{
    struct
    {
        USB_ConfigurationDescriptor_t configuration;
        USB_InterfaceDescriptor_t interface0;
        USB_EndpointDescriptor_t endpoint0out;
    } __attribute__((packed, aligned(1)));
    uint8_t raw[18];
} __attribute__((packed, aligned(1))) USB_WholeDescriptor_t;


typedef enum
{
    USB_DEVICE_DESCRIPTOR = 1,
    USB_CONFIGURATION_DESCRIPTOR,
    USB_STRING_DESCRIPTOR,
    USB_INTERFACE_DESCRIPTOR,
    USB_ENDPOINT_DESCRIPTOR
} __attribute__((packed)) USB_DescriptorType_t;

#define USB_STRING_DESCRIPTOR_LENGTH(...)       (sizeof((uint16_t[]){__VA_ARGS__}) + 2)
#define USB_BUILD_STRING_DESCRIPTOR(...)        {USB_STRING_DESCRIPTOR_LENGTH(__VA_ARGS__) | (USB_STRING_DESCRIPTOR << 8), __VA_ARGS__}

extern const uint16_t USB_StringDescriptor_LangID[];
extern const uint16_t USB_StringDescriptor_Vendor[];
extern const uint16_t USB_StringDescriptor_Product[];
extern const uint16_t USB_StringDescriptor_Serial[];

extern const USB_DeviceDescriptor_t USB_DeviceDescriptor;
// extern const USB_ConfigurationDescriptor_t USB_ConfigurationDescriptor;
// extern const USB_InterfaceDescriptor_t USB_InterfaceDescriptor;
extern const USB_WholeDescriptor_t USB_ConfigurationInterfaceDescriptor;
