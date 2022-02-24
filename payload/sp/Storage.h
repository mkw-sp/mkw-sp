#pragma once

#include <Common.h>

typedef struct {
    u32 fd;
} Node;

typedef struct {
    Node;
} Dir;

typedef struct {
    Node;
} File;

enum {
    MODE_READ = 1 << 0,
    MODE_WRITE = 1 << 1,
};

typedef struct {
    bool (*open)(File *file, const char *path, u32 mode);
    bool (*close)(File *file);
    bool (*read)(File *file, void *dst, u32 size, u32 *readSize);
    bool (*write)(File *file, const void *src, u32 size, u32 *writtenSize);
    u64 (*size)(File *file);
    bool (*lseek)(File *file, u64 offset);
    u64 (*tell)(File *file);
} Storage;

bool Storage_init(void);

bool Storage_open(File *file, const char *path, u32 mode);

bool Storage_close(File *file);

bool Storage_read(File *file, void *dst, u32 size, u32 *readSize);

bool Storage_write(File *file, const void *src, u32 size, u32 *writtenSize);

u64 Storage_size(File *file);

bool Storage_lseek(File *file, u64 offset);

u64 Storage_tell(File *file);
