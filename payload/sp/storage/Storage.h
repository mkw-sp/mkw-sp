#pragma once

#include <Common.h>

typedef struct Node {
    struct Storage* storage;
    u32 fd;
} Node;

typedef struct {
    struct Node;
} File;

typedef struct {
    struct Node;
} Dir;

typedef struct {
    struct Storage *storage;
    u64 id;
} NodeId;

enum {
    NODE_TYPE_NONE = 0x0,
    NODE_TYPE_FILE = 0x1,
    NODE_TYPE_DIR = 0x2,
};

typedef struct {
    NodeId id;
    u32 type;
    u64 size;
    wchar_t name[255 + 1];
} NodeInfo;

typedef struct Storage {
    struct Storage* next;
    bool (*fastOpen)(File *file, u64 id);
    bool (*open)(File *file, const wchar_t *path, const char *mode);
    bool (*close)(File *file);
    bool (*read)(File *file, void *dst, u32 size, u32 offset);
    bool (*write)(File *file, const void *src, u32 size, u32 offset);
    bool (*sync)(File *file);
    u64 (*size)(File *file);
    bool (*createDir)(const wchar_t *path, bool allowNop);
    bool (*fastOpenDir)(Dir *dir, u64 id);
    bool (*openDir)(Dir *dir, const wchar_t *path);
    bool (*readDir)(Dir *dir, NodeInfo *info);
    bool (*closeDir)(Dir *dir);
    void (*stat)(const wchar_t *path, NodeInfo *info);
    bool (*rename)(const wchar_t *srcPath, const wchar_t *dstPath);
    bool (*remove)(const wchar_t *path, bool allowNop);
} Storage;

bool Storage_init(void);

bool Storage_fastOpen(File *file, NodeId id);

bool Storage_open(File *file, const wchar_t *path, const char *mode);

bool Storage_close(File *file);

bool Storage_read(File *file, void *dst, u32 size, u32 offset);

bool Storage_write(File *file, const void *src, u32 size, u32 offset);

bool Storage_sync(File *file);

u64 Storage_size(File *file);

bool Storage_fastReadFile(NodeId id, void *dst, u32 size, u32 *readSize);

bool Storage_readFile(const wchar_t *path, void *dst, u32 size, u32 *readSize);

bool Storage_writeFile(const wchar_t *path, bool overwrite, const void *src, u32 size);

bool Storage_createDir(const wchar_t *path, bool allowNop);

bool Storage_fastOpenDir(Dir *dir, NodeId);

bool Storage_openDir(Dir *dir, const wchar_t *path);

bool Storage_readDir(Dir *dir, NodeInfo *info);

bool Storage_closeDir(Dir *dir);

void Storage_stat(const wchar_t *path, NodeInfo *info);

bool Storage_rename(const wchar_t *srcPath, const wchar_t *dstPath);

bool Storage_remove(const wchar_t *path, bool allowNop);
