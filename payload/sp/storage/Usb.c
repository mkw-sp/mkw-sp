// Resources:
// - https://wiibrew.org/wiki//dev/usb/ven
// - https://github.com/dolphin-emu/dolphin/blob/master/Source/Core/Core/IOS/USB/USB_VEN/VEN.cpp
// - http://www.usb.org/developers/docs/usb_20.zip
// - https://www.beyondlogic.org/usbnutshell/usb1.shtml
// - https://libusb.sourceforge.io/api-1.0/annotated.html
// - https://github.com/devkitPro/libogc/blob/master/libogc/usb.c

#include "Usb.h"

#include <sp/Bytes.h>
#include <sp/ScopeLock.h>

#include <revolution.h>
#include <revolution/ios.h>

#include <string.h>

enum {
    MAX_DEVICE_COUNT = 0x20,
};

enum {
    IOCTL_GET_VERSION = 0x0,
    IOCTL_GET_DEVICE_CHANGE = 0x1,
    IOCTL_GET_DEVICE_INFO = 0x3,
    IOCTL_ATTACH_FINISH = 0x6,
    IOCTL_SET_ALTERNATE_SETTING = 0x7,
    IOCTL_SUSPEND_RESUME = 0x10,
};

enum {
    IOCTLV_CTRL_TRANSFER = 0x12,
    IOCTLV_BULK_TRANSFER = 0x15,
};

typedef struct {
    u32 id;
    u8 _04[0x08 - 0x04];
    union {
        struct {
            u8 requestType;
            u8 request;
            u16 value;
            u16 index;
            u16 length;
            void *data;
        } ctrl;
        struct {
            void *data;
            u16 length;
            u8 _0e[0x12 - 0x0e];
            u8 endpoint;
        } bulk;
    };
    u8 _14[0x20 - 0x14];
    IOVector vec[4];
} Transfer;
static_assert(sizeof(Transfer) == 0x40);

typedef struct {
    u32 id;
    UsbHandler *handler;
    OSMutex mutex;
    Transfer *transfer;
} Device;

typedef struct {
    Device **device;
    u32 id;
} DeviceGuard;

typedef struct {
    u32 id;
    u16 vendorId;
    u16 productId;
    u8 _8[0xa - 0x8];
    u8 interfaceNumber;
    u8 alternateSettingCount;
} DeviceEntry;
static_assert(sizeof(DeviceEntry) == 0xc);

static OSMutex mutex;
static OSMessage message;
static OSMessageQueue queue;
static u8 stack[0x2000] = {0}; // 8 KiB
static OSThread thread;
static bool isInit;
static s32 fd = -1;
static UsbHandler *head = NULL;
static Device devices[MAX_DEVICE_COUNT];
static u8 *buffer;
static u32 deviceEntryCount;
static DeviceEntry *deviceEntries;

static void Usb_lockDevice(DeviceGuard *guard) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    for (u32 i = 0; i < MAX_DEVICE_COUNT; i++) {
        if (!devices[i].handler) {
            continue;
        }
        if (devices[i].id == guard->id) {
            OSLockMutex(&devices[i].mutex);
            *guard->device = &devices[i];
            return;
        }
    }
}

static void Usb_unlockDevice(DeviceGuard *guard) {
    if (!*guard->device) {
        return;
    }

    OSUnlockMutex(&(*guard->device)->mutex);
}

#define LOCK_DEVICE(device, id) \
    DeviceGuard _guard __attribute__((cleanup(Usb_unlockDevice))) = {&device, id}; \
    Usb_lockDevice(&_guard)

static void Usb_printDeviceEntry(u32 i) {
    const DeviceEntry *entry = &deviceEntries[i];
    SP_LOG("DeviceEntry(%x):", i);
    SP_LOG("    id: %x", entry->id);
    SP_LOG("    vendorId: %x", entry->vendorId);
    SP_LOG("    productId: %x", entry->productId);
    SP_LOG("    interfaceNumber: %x", entry->interfaceNumber);
    SP_LOG("    alternateSettingCount: %x", entry->alternateSettingCount);
}

static bool Usb_wasDeviceRemoved(u32 i) {
    if (!devices[i].handler) {
        return false;
    }

    for (u32 j = 0; j < deviceEntryCount; j++) {
        if (deviceEntries[j].id == devices[i].id) {
            return false;
        }
    }

    return true;
}

static void Usb_handleRemoval(u32 i) {
    if (!Usb_wasDeviceRemoved(i)) {
        return;
    }

    SP_SCOPED_MUTEX_LOCK(devices[i].mutex);
    devices[i].handler->onDeviceRemove(devices[i].id);
    devices[i].handler = NULL;
}

static bool Usb_wasDeviceAdded(u32 i) {
    for (u32 j = 0; j < MAX_DEVICE_COUNT; j++) {
        if (!devices[j].handler) {
            continue;
        }
        if (devices[j].id == deviceEntries[i].id) {
            return false;
        }
    }

    return true;
}

static u32 Usb_findSlot(void) {
    for (u32 i = 0; i < MAX_DEVICE_COUNT; i++) {
        if (!devices[i].handler) {
            return i;
        }
    }

    panic("No slot found");
}

static void Usb_handleAddition(u32 i) {
    if (!Usb_wasDeviceAdded(i)) {
        return;
    }

    u32 j = Usb_findSlot();
    SP_SCOPED_MUTEX_LOCK(devices[j].mutex);
    devices[j].id = deviceEntries[i].id;

    memset(buffer, 0, 0x20);
    write_u32(buffer, 0x0, deviceEntries[i].id);
    write_u8(buffer, 0xb, 1);
    if (IOS_Ioctl(fd, IOCTL_SUSPEND_RESUME, buffer, 0x20, NULL, 0) < 0) {
        return;
    }

    for (u32 k = 0; k < deviceEntries[i].alternateSettingCount && !devices[j].handler; k++) {
        memset(buffer, 0, 0x20 + 0xc0);
        write_u32(buffer, 0x0, deviceEntries[i].id);
        write_u8(buffer, 0x8, k);
        if (IOS_Ioctl(fd, IOCTL_GET_DEVICE_INFO, buffer, 0x20, buffer + 0x20, 0xc0) >= 0) {
            const UsbDeviceInfo *deviceInfo = (UsbDeviceInfo *)(buffer + 0x20);
            for (UsbHandler *handler = head; handler; handler = handler->next) {
                devices[j].handler = handler;
                if (!handler->onDeviceAdd(deviceInfo)) {
                    devices[j].handler = NULL;
                    continue;
                }

                memset(buffer, 0, 0x20 + 0xc0);
                write_u32(buffer, 0x0, deviceEntries[i].id);
                write_u8(buffer, 0x8, k);
                if (IOS_Ioctl(fd, IOCTL_SET_ALTERNATE_SETTING, buffer, 0x20, NULL, 0) >= 0) {
                    return;
                }

                handler->onDeviceRemove(deviceEntries[i].id);
                devices[j].handler = NULL;
            }
        }
    }

    memset(buffer, 0, 0x20);
    write_u32(buffer, 0x0, deviceEntries[i].id);
    IOS_Ioctl(fd, IOCTL_SUSPEND_RESUME, buffer, 0x20, NULL, 0);
}

static void Usb_updateDevices(void) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    for (u32 i = 0; i < deviceEntryCount; i++) {
        Usb_printDeviceEntry(i);
    }

    for (u32 i = 0; i < deviceEntryCount; i++) {
        for (u32 j = i + 1; j < deviceEntryCount; j++) {
            assert(deviceEntries[i].id != deviceEntries[j].id);
        }
    }

    for (u32 i = 0; i < MAX_DEVICE_COUNT; i++) {
        Usb_handleRemoval(i);
    }

    for (u32 i = 0; i < deviceEntryCount; i++) {
        Usb_handleAddition(i);
    }

    IOS_Ioctl(fd, IOCTL_ATTACH_FINISH, NULL, 0, NULL, 0);

    if (!isInit) {
        isInit = true;
        OSSendMessage(&queue, NULL, OS_MESSAGE_BLOCK);
    }
}

static void *Usb_discoverDevices(void * /* arg */) {
    while (true) {
        s32 result = IOS_Ioctl(fd, IOCTL_GET_DEVICE_CHANGE, NULL, 0, deviceEntries,
                MAX_DEVICE_COUNT * sizeof(DeviceEntry));
        assert(result >= 0 && result <= MAX_DEVICE_COUNT);

        deviceEntryCount = result;
        Usb_updateDevices();
    }
}

bool Usb_init(void) {
    OSInitMutex(&mutex);

    for (u32 i = 0; i < MAX_DEVICE_COUNT; i++) {
        devices[i].handler = NULL;
        OSInitMutex(&devices[i].mutex);
        devices[i].transfer = OSAllocFromMEM2ArenaLo(sizeof(Transfer), 0x20);
    }
    buffer = OSAllocFromMEM2ArenaLo(0xe0, 0x20);
    deviceEntries = OSAllocFromMEM2ArenaLo(MAX_DEVICE_COUNT * sizeof(DeviceEntry), 0x20);

    fd = IOS_Open("/dev/usb/ven", 0);
    if (fd < 0) {
        SP_LOG("Failed to open /dev/usb/ven: Returned error %i", fd);
        return false;
    }
    SP_LOG("Successfully opened interface: ID: %i", fd);

    if (IOS_Ioctl(fd, IOCTL_GET_VERSION, NULL, 0, buffer, 0x20) < 0) {
        SP_LOG("Failed to get version");
        IOS_Close(fd);
        return false;
    }
    u32 version = read_u32(buffer, 0x0);
    if (version != 0x50001) {
        SP_LOG("Got unexpected version %x", version);
        IOS_Close(fd);
        return false;
    }

    OSInitMessageQueue(&queue, &message, 1);
    OSCreateThread(&thread, Usb_discoverDevices, NULL, stack + sizeof(stack), sizeof(stack), 20, 0);
    OSResumeThread(&thread);
    OSReceiveMessage(&queue, NULL, OS_MESSAGE_BLOCK);

    return true;
}

void Usb_addHandler(UsbHandler *handler) {
    assert(isInit);
    assert(handler);

    SP_SCOPED_MUTEX_LOCK(mutex);

    UsbHandler *next = head;
    handler->next = NULL;
    head = handler;
    for (u32 i = 0; i < deviceEntryCount; i++) {
        Usb_handleAddition(i);
    }
    handler->next = next;
}

bool Usb_removeHandler(UsbHandler *handler) {
    assert(isInit);
    assert(handler);

    SP_SCOPED_MUTEX_LOCK(mutex);

    for (u32 i = 0; i < MAX_DEVICE_COUNT; i++) {
        if (devices[i].handler == handler) {
            return false;
        }
    }

    for (UsbHandler **next = &head; *next; next = &(*next)->next) {
        if (*next == handler) {
            *next = (*next)->next;
            break;
        }
    }

    return true;
}

bool Usb_ctrlTransfer(u32 id, u8 requestType, u8 request, u16 value, u16 index, u16 length,
        void *data) {
    assert(isInit);
    assert(!!length == !!data);
    assert(!((u32)data & 0x1f));

    Device *device = NULL;
    LOCK_DEVICE(device, id);
    if (!device) {
        return false;
    }

    Transfer *transfer = device->transfer;
    memset(transfer, 0, sizeof(*transfer));

    transfer->id = device->id;
    transfer->ctrl.requestType = requestType;
    transfer->ctrl.request = request;
    transfer->ctrl.value = value;
    transfer->ctrl.index = index;
    transfer->ctrl.length = length;
    transfer->ctrl.data = data;

    transfer->vec[0].data = transfer;
    transfer->vec[0].size = sizeof(*transfer);
    transfer->vec[1].data = data;
    transfer->vec[1].size = length;

    u8 direction = requestType >> USB_ENDPOINT_DIRECTION_SHIFT & USB_ENDPOINT_DIRECTION_MASK;
    u32 inputCount = direction == USB_ENDPOINT_DIRECTION_HOST_TO_DEVICE ? 2 : 1;
    u32 outputCount = direction == USB_ENDPOINT_DIRECTION_HOST_TO_DEVICE ? 0 : 1;
    s32 result = IOS_Ioctlv(fd, IOCTLV_CTRL_TRANSFER, inputCount, outputCount, transfer->vec);
    return result == 0x8 + length;
}

bool Usb_bulkTransfer(u32 id, u8 endpoint, u16 length, void *data) {
    assert(isInit);
    assert(!!length == !!data);
    assert(!((u32)data & 0x1f));

    Device *device = NULL;
    LOCK_DEVICE(device, id);
    if (!device) {
        return false;
    }

    Transfer *transfer = device->transfer;
    memset(transfer, 0, sizeof(*transfer));

    transfer->id = device->id;
    transfer->bulk.data = data;
    transfer->bulk.length = length;
    transfer->bulk.endpoint = endpoint;

    transfer->vec[0].data = transfer;
    transfer->vec[0].size = sizeof(*transfer);
    transfer->vec[1].data = data;
    transfer->vec[1].size = length;

    u8 direction = endpoint >> USB_ENDPOINT_DIRECTION_SHIFT & USB_ENDPOINT_DIRECTION_MASK;
    u32 inputCount = direction == USB_ENDPOINT_DIRECTION_HOST_TO_DEVICE ? 2 : 1;
    u32 outputCount = direction == USB_ENDPOINT_DIRECTION_HOST_TO_DEVICE ? 0 : 1;
    s32 result = IOS_Ioctlv(fd, IOCTLV_BULK_TRANSFER, inputCount, outputCount, transfer->vec);
    return result == length;
}
