#pragma once

#include <Common.h>

typedef enum {
    NHTTP_ERROR_NONE,
} NHTTPError;

typedef enum {
    NHTTP_REQUEST_METHOD_GET,
} NHTTPRequestMethod;

typedef enum {
    NHTTP_STATUS_CODE_OK = 200,
    NHTTP_STATUS_CODE_BAD_REQUEST = 400,
    NHTTP_STATUS_CODE_INTERNAL_SERVER_ERROR = 500,
} NHTTPStatusCode;

typedef void *NHTTPRequestHandle;
typedef void *NHTTPResponseHandle;

typedef void *(*NHTTPAlloc)(u32 size, int align);
typedef void (*NHTTPFree)(void *block);
typedef void (*NHTTPCleanupCallback)(void);
typedef void (
        *NHTTPRequestCallback)(NHTTPError error, NHTTPResponseHandle responseHandle, void *arg);

int NHTTPStartup(NHTTPAlloc alloc, NHTTPFree free, u32 threadPriority);
void NHTTPCleanupAsync(NHTTPCleanupCallback cleanupCallback);
NHTTPRequestHandle NHTTPCreateRequest(const char *url, NHTTPRequestMethod requestMethod,
        char *responseBuffer, u32 responseBufferSize, NHTTPRequestCallback requestCallback,
        void *arg);
int NHTTPSendRequestAsync(NHTTPRequestHandle requestHandle);
int NHTTPCancelRequestAsync(int requestId);
void NHTTPDestroyResponse(NHTTPResponseHandle responseHandle);
int NHTTPGetBodyAll(NHTTPResponseHandle responseHandle, char **responseBody);
int NHTTPGetResultCode(NHTTPResponseHandle responseHandle);
