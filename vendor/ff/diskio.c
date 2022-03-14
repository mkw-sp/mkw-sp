/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"         /* Obtains integer types */
#include "diskio.h"     /* Declarations of disk functions */

#include "sp/FatStorage.h"


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE UNUSED(pdrv)   /* Physical drive nmuber to identify the drive */
)
{
    return 0;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE UNUSED(pdrv)   /* Physical drive nmuber to identify the drive */
)
{
    return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE UNUSED(pdrv),  /* Physical drive nmuber to identify the drive */
    BYTE *buff,         /* Data buffer to store read data */
    LBA_t sector,       /* Start sector in LBA */
    UINT count          /* Number of sectors to read */
)
{
    return FatStorage_diskRead(sector, count, buff) ? RES_OK : RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE UNUSED(pdrv),  /* Physical drive nmuber to identify the drive */
    const BYTE *buff,   /* Data to be written */
    LBA_t sector,       /* Start sector in LBA */
    UINT count          /* Number of sectors to write */
)
{
    return FatStorage_diskWrite(sector, count, buff) ? RES_OK : RES_ERROR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE UNUSED(pdrv),  /* Physical drive nmuber (0..) */
    BYTE cmd,           /* Control code */
    void *buff          /* Buffer to send/receive control data */
)
{
    switch (cmd) {
    case CTRL_SYNC:
        return FatStorage_diskSync() ? RES_OK : RES_ERROR;
    case GET_SECTOR_SIZE:
        *(WORD *)buff = FatStorage_diskSectorSize();
        return RES_OK;
    case CTRL_TRIM:;
        const LBA_t *args = buff;
        LBA_t firstSector = args[0];
        LBA_t sectorCount = args[1] - args[0] + 1;
        return FatStorage_diskErase(firstSector, sectorCount) ? RES_OK : RES_ERROR;
    default:
        assert(false);
    }
}
