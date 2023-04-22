// clang-format off
//
// Resources:
// - https://www.usb.org/sites/default/files/usbmassbulk_10.pdf
// - https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// - https://www.downtowndougbrown.com/2018/12/usb-mass-storage-with-embedded-devices-tips-and-quirks/
// - https://github.com/devkitPro/libogc/blob/master/libogc/usbstorage.c
//
// clang-format on

#include "UsbStorage.h"

#include <sp/Bytes.h>

#include "Usb.h"

#include <revolution.h>

#include <string.h>

enum {
    MSC_GET_MAX_LUN = 0xfe,
};

enum {
    CSW_SIZE = 0x1f,
    CBW_SIZE = 0xd,
};

enum {
    SCSI_TEST_UNIT_READY = 0x0,
    SCSI_REQUEST_SENSE = 0x3,
    SCSI_INQUIRY = 0x12,
    SCSI_READ_CAPACITY_10 = 0x25,
    SCSI_READ_10 = 0x28,
    SCSI_WRITE_10 = 0x2a,
    SCSI_SYNCHRONIZE_CACHE_10 = 0x35,
};

enum {
    SCSI_TYPE_DIRECT_ACCESS = 0x0,
};

static bool deviceFound = false;
static u32 id;
static u8 interface;
static u8 outEndpoint;
static u8 inEndpoint;
static u8 *buffer = NULL;
static u32 tag = 0;
static u8 lun;
static u32 blockSize;

static bool UsbStorage_getLunCount(u8 *lunCount) {
    u8 requestType = 0;
    requestType |= USB_RECIPIENT_INTERFACE << USB_RECIPIENT_SHIFT;
    requestType |= USB_REQUEST_TYPE_CLASS << USB_REQUEST_TYPE_SHIFT;
    requestType |= USB_ENDPOINT_DIRECTION_DEVICE_TO_HOST << USB_ENDPOINT_DIRECTION_SHIFT;
    if (!Usb_ctrlTransfer(id, requestType, MSC_GET_MAX_LUN, 0, interface, 0x1, buffer)) {
        return false;
    }
    *lunCount = read_u8(buffer, 0x0) + 1;
    return *lunCount >= 1 && *lunCount <= 16;
}

static bool UsbStorage_scsiTransfer(bool isWrite, u32 size, void *data, u8 lun, u8 cbSize,
        void *cb) {
    assert(!!size == !!data);
    assert(lun <= 16);
    assert(cbSize >= 1 && cbSize <= 16);
    assert(cb);

    memset(buffer, 0, CSW_SIZE);

    tag++;

    write_u32_le(buffer, 0x0, 0x43425355);
    write_u32_le(buffer, 0x4, tag);
    write_u32_le(buffer, 0x8, size);
    write_u8(buffer, 0xc, !isWrite << 7);
    write_u8(buffer, 0xd, lun);
    write_u8(buffer, 0xe, cbSize);
    memcpy(buffer + 0xf, cb, cbSize);

    if (!Usb_bulkTransfer(id, outEndpoint, CSW_SIZE, buffer)) {
        return false;
    }

    u32 remainingSize = size;
    while (remainingSize > 0) {
        u32 chunkSize = MIN(remainingSize, 0x4000);
        if (isWrite) {
            memcpy(buffer, data, chunkSize);
        }
        if (!Usb_bulkTransfer(id, isWrite ? outEndpoint : inEndpoint, chunkSize, buffer)) {
            return false;
        }
        if (!isWrite) {
            memcpy(data, buffer, chunkSize);
        }
        remainingSize -= chunkSize;
        data += chunkSize;
    }

    memset(buffer, 0, CBW_SIZE);

    if (!Usb_bulkTransfer(id, inEndpoint, CBW_SIZE, buffer)) {
        return false;
    }

    if (read_u32_le(buffer, 0x0) != 0x53425355) {
        return false;
    }
    if (read_u32_le(buffer, 0x4) != tag) {
        return false;
    }
    if (read_u32_le(buffer, 0x8) != 0) {
        return false;
    }
    if (read_u8(buffer, 0xc) != 0) {
        return false;
    }

    return true;
}

static bool UsbStorage_testUnitReady(u8 lun) {
    u8 cmd[6] = {0};
    write_u8(cmd, 0x0, SCSI_TEST_UNIT_READY);

    return UsbStorage_scsiTransfer(false, 0, NULL, lun, sizeof(cmd), cmd);
}

static bool UsbStorage_inquiry(u8 lun, u8 *type) {
    u8 response[36] = {0};
    u8 cmd[6] = {0};
    write_u8(cmd, 0x0, SCSI_INQUIRY);
    write_u8(cmd, 0x1, lun << 5);
    write_u8(cmd, 0x4, sizeof(response));

    if (!UsbStorage_scsiTransfer(false, sizeof(response), response, lun, sizeof(cmd), cmd)) {
        return false;
    }

    *type = response[0] & 0x1f;
    return true;
}

static bool UsbStorage_initLun(u8 lun) {
    if (!UsbStorage_testUnitReady(lun)) {
        return false;
    }

    u8 type;
    if (!UsbStorage_inquiry(lun, &type)) {
        return false;
    }
    return type == SCSI_TYPE_DIRECT_ACCESS;
}

static bool UsbStorage_requestSense(u8 lun) {
    u8 response[18] = {0};
    u8 cmd[6] = {0};
    write_u8(cmd, 0x0, SCSI_REQUEST_SENSE);
    write_u8(cmd, 0x4, sizeof(response));

    if (!UsbStorage_scsiTransfer(false, sizeof(response), response, lun, sizeof(cmd), cmd)) {
        return false;
    }

    SP_LOG("Sense key: %x", response[0x2] & 0xf);
    return true;
}

static bool UsbStorage_findLun(u8 lunCount, u8 *lun) {
    for (*lun = 0; *lun < lunCount; (*lun)++) {
        for (u32 try = 0; try < 5; try++) {
            if (UsbStorage_initLun(*lun)) {
                return true;
            }

            // This can clear a UNIT ATTENTION condition
            UsbStorage_requestSense(*lun);

            OSSleepMilliseconds(try * 10);
        }
    }

    return false;
}

static bool UsbStorage_readCapacity(u8 lun, u32 *blockSize) {
    u8 response[8] = {0};
    u8 cmd[10] = {0};
    write_u8(cmd, 0x0, SCSI_READ_CAPACITY_10);

    if (!UsbStorage_scsiTransfer(false, sizeof(response), response, lun, sizeof(cmd), cmd)) {
        return false;
    }

    *blockSize = read_u32(response, 0x4);
    return true;
}

static bool UsbStorage_onDeviceAdd(const UsbDeviceInfo *info) {
    if (deviceFound) {
        return false;
    }

    u8 interfaceClass = info->interfaceDescriptor.interfaceClass;
    if (interfaceClass != USB_INTERFACE_CLASS_MASS_STORAGE) {
        return false;
    }

    u8 interfaceSubClass = info->interfaceDescriptor.interfaceSubClass;
    if (interfaceSubClass != USB_INTERFACE_SUB_CLASS_MASS_STORAGE_SCSI) {
        return false;
    }

    u8 interfaceProtocol = info->interfaceDescriptor.interfaceProtocol;
    if (interfaceProtocol != USB_INTERFACE_PROTOCOL_MASS_STORAGE_BULK_ONLY) {
        return false;
    }

    u8 numEndpoints = info->interfaceDescriptor.numEndpoints;
    assert(numEndpoints <= 16);
    bool outFound = false;
    bool inFound = false;
    for (u32 i = 0; i < numEndpoints; i++) {
        const UsbEndpointDescriptor *endpointDescriptor = &info->endpointDescriptors[i];

        u8 transferType = endpointDescriptor->attributes >> USB_ENDPOINT_TRANSFER_TYPE_SHIFT &
                USB_ENDPOINT_TRANSFER_TYPE_MASK;
        if (transferType != USB_ENDPOINT_TRANSFER_TYPE_BULK) {
            continue;
        }

        u8 direction = endpointDescriptor->endpointAddress >> USB_ENDPOINT_DIRECTION_SHIFT &
                USB_ENDPOINT_DIRECTION_MASK;
        if (!outFound && direction == USB_ENDPOINT_DIRECTION_HOST_TO_DEVICE) {
            outEndpoint = endpointDescriptor->endpointAddress;
            outFound = true;
        }
        if (!inFound && direction == USB_ENDPOINT_DIRECTION_DEVICE_TO_HOST) {
            inEndpoint = endpointDescriptor->endpointAddress;
            inFound = true;
        }
    }
    if (!outFound || !inFound) {
        return false;
    }

    u16 vendorId = info->deviceDescriptor.vendorId;
    u16 productId = info->deviceDescriptor.productId;
    SP_LOG("Found device %x:%x", vendorId, productId);
    id = info->id;
    interface = info->interfaceDescriptor.interfaceNumber;

    u8 lunCount;
    if (!UsbStorage_getLunCount(&lunCount)) {
        return false;
    }
    SP_LOG("Device has %d logical unit(s)", lunCount);

    if (!UsbStorage_findLun(lunCount, &lun)) {
        return false;
    }
    SP_LOG("Using logical unit %d", lun);

    if (!UsbStorage_readCapacity(lun, &blockSize)) {
        return false;
    }
    SP_LOG("Block size: %d bytes", blockSize);

    if (!UsbStorage_testUnitReady(lun)) {
        return false;
    }

    deviceFound = true;
    return true;
}

__attribute__((noreturn)) static void UsbStorage_onDeviceRemove(u32 /* id */) {
    panic("Device was removed!");
}

static UsbHandler handler = {
        .onDeviceAdd = UsbStorage_onDeviceAdd,
        .onDeviceRemove = UsbStorage_onDeviceRemove,
};

static u32 UsbStorage_sectorSize(void) {
    return blockSize;
}

static bool UsbStorage_read(u32 firstSector, u32 sectorCount, void *buffer) {
    assert(sectorCount <= UINT16_MAX);

    for (u32 try = 0; try < 5; try++) {
        u8 cmd[10] = {0};
        write_u8(cmd, 0x0, SCSI_READ_10);
        write_u32(cmd, 0x2, firstSector);
        write_u16(cmd, 0x7, sectorCount);

        u32 size = sectorCount * blockSize;
        if (UsbStorage_scsiTransfer(false, size, buffer, lun, sizeof(cmd), cmd)) {
            return true;
        }

        OSSleepMilliseconds(try * 10);
    }

    return false;
}

static bool UsbStorage_write(u32 firstSector, u32 sectorCount, const void *buffer) {
    assert(sectorCount <= UINT16_MAX);

    for (u32 try = 0; try < 5; try++) {
        u8 cmd[10] = {0};
        write_u8(cmd, 0x0, SCSI_WRITE_10);
        write_u32(cmd, 0x2, firstSector);
        write_u16(cmd, 0x7, sectorCount);

        u32 size = sectorCount * blockSize;
        if (UsbStorage_scsiTransfer(true, size, (void *)buffer, lun, sizeof(cmd), cmd)) {
            return true;
        }

        OSSleepMilliseconds(try * 10);
    }

    return false;
}

static bool UsbStorage_erase(u32 /* firstSector */, u32 /* sectorCount */) {
    // This is not supported under Bulk Only Transport
    return true;
}

static bool UsbStorage_sync(void) {
    u8 cmd[10] = {0};
    write_u8(cmd, 0x0, SCSI_SYNCHRONIZE_CACHE_10);

    return UsbStorage_scsiTransfer(false, 0, NULL, lun, sizeof(cmd), cmd);
}

static u32 UsbStorage_getMessageId(void) {
    return 10156;
}

static const FATStorage usbStorage = {
        UsbStorage_sectorSize,
        UsbStorage_read,
        UsbStorage_write,
        UsbStorage_erase,
        UsbStorage_sync,
        UsbStorage_getMessageId,
};

bool UsbStorage_init(const FATStorage **fatStorage) {
    if (!buffer) {
        buffer = OSAllocFromMEM2ArenaLo(0x4000, 0x20);
    }

    Usb_addHandler(&handler);

    if (Usb_removeHandler(&handler)) {
        return false;
    }

    *fatStorage = &usbStorage;

    SP_LOG("Successfully completed initialization");
    return true;
}
