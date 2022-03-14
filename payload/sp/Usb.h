#pragma once

#include <Common.h>

enum {
    USB_INTERFACE_CLASS_MASS_STORAGE = 0x8,
};

enum {
    USB_INTERFACE_SUB_CLASS_MASS_STORAGE_SCSI = 0x6,
};

enum {
    USB_INTERFACE_PROTOCOL_MASS_STORAGE_BULK_ONLY = 0x50,
};

enum {
    USB_ENDPOINT_DIRECTION_SHIFT = 7,
    USB_ENDPOINT_DIRECTION_MASK = 0x1,
    USB_ENDPOINT_DIRECTION_HOST_TO_DEVICE = 0x0,
    USB_ENDPOINT_DIRECTION_DEVICE_TO_HOST = 0x1,
};

enum {
    USB_ENDPOINT_TRANSFER_TYPE_SHIFT = 0,
    USB_ENDPOINT_TRANSFER_TYPE_MASK = 0x3,
    USB_ENDPOINT_TRANSFER_TYPE_CONTROL = 0x0,
    USB_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS = 0x1,
    USB_ENDPOINT_TRANSFER_TYPE_BULK = 0x2,
    USB_ENDPOINT_TRANSFER_TYPE_INTERRUPT = 0x3,
};

enum {
    USB_RECIPIENT_SHIFT = 0,
    USB_RECIPIENT_MASK = 0x1f,
    USB_RECIPIENT_DEVICE = 0x0,
    USB_RECIPIENT_INTERFACE = 0x1,
    USB_RECIPIENT_ENDPOINT = 0x2,
    USB_RECIPIENT_OTHER = 0x3,
};

enum {
    USB_REQUEST_TYPE_SHIFT = 5,
    USB_REQUEST_TYPE_MASK = 0x3,
    USB_REQUEST_TYPE_STANDARD = 0x0,
    USB_REQUEST_TYPE_CLASS = 0x1,
    USB_REQUEST_TYPE_VENDOR = 0x2,
    USB_REQUEST_TYPE_RESERVED = 0x3,
};

typedef struct {
    u8 length;
    u8 type;
    u16 usbVersion;
    u8 deviceClass;
    u8 deviceSubClass;
    u8 deviceProtocol;
    u8 maxPacketSize0;
    u16 vendorId;
    u16 productId;
    u16 deviceVersion;
    u8 manufacturerIndex;
    u8 productIndex;
    u8 serialNumberIndex;
    u8 numConfigurations;
    u8 _12[0x14 - 0x12];
} UsbDeviceDescriptor;
static_assert(sizeof(UsbDeviceDescriptor) == 0x14);

typedef struct {
    u8 length;
    u8 type;
    u16 totalLength;
    u8 numInterfaces;
    u8 configurationValue;
    u8 configurationIndex;
    u8 attributes;
    u8 maxPower;
    u8 _9[0xc - 0x9];
} UsbConfigDescriptor;
static_assert(sizeof(UsbConfigDescriptor) == 0xc);

typedef struct {
    u8 length;
    u8 type;
    u8 interfaceNumber;
    u8 alternateSetting;
    u8 numEndpoints;
    u8 interfaceClass;
    u8 interfaceSubClass;
    u8 interfaceProtocol;
    u8 interfaceIndex;
    u8 _9[0xc - 0x9];
} UsbInterfaceDescriptor;
static_assert(sizeof(UsbInterfaceDescriptor) == 0xc);

typedef struct {
    u8 length;
    u8 type;
    u8 endpointAddress;
    u8 attributes;
    u16 maxPacketSize;
    u8 interval;
    u8 _7[0x8 - 0x7];
} UsbEndpointDescriptor;
static_assert(sizeof(UsbEndpointDescriptor) == 0x8);

typedef struct {
    u32 id;
    u8 _04[0x14 - 0x04];
    UsbDeviceDescriptor deviceDescriptor;
    UsbConfigDescriptor configDescriptor;
    UsbInterfaceDescriptor interfaceDescriptor;
    UsbEndpointDescriptor endpointDescriptors[16];
} UsbDeviceInfo;
static_assert(sizeof(UsbDeviceInfo) == 0xc0);

typedef bool (*Usb_onDeviceAddFunc)(const UsbDeviceInfo *info);
typedef void (*Usb_onDeviceRemoveFunc)(u32 id);

typedef struct UsbHandler UsbHandler;

struct UsbHandler {
    Usb_onDeviceAddFunc onDeviceAdd;
    Usb_onDeviceRemoveFunc onDeviceRemove;
    UsbHandler *next;
};

bool Usb_init(void);

void Usb_addHandler(UsbHandler *handler);

void Usb_removeHandler(UsbHandler *handler);

bool Usb_ctrlTransfer(u32 id, u8 requestType, u8 request, u16 value, u16 index, u16 length,
        void *data);

bool Usb_bulkTransfer(u32 id, u8 endpoint, u16 length, void *data);
