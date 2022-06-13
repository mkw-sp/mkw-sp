#include <revolution.h>

#include <stdalign.h>

static int
hydro_random_init(void)
{
    if (ESP_InitLib() < 0) {
        return -1;
    }

    alignas(0x40) u8 signature[0x3c];
    alignas(0x40) u8 certificate[0x180];
    if (ESP_Sign(NULL, 0, signature, certificate) < 0) {
        return -1;
    }

    memcpy(hydro_random_context.state, signature + 4, gimli_BLOCKBYTES);
    memcpy(&hydro_random_context.counter, signature + 4 + gimli_BLOCKBYTES, 8);
    hydro_memzero(signature, sizeof(signature));
    hydro_memzero(certificate, sizeof(certificate));
    return 0;
}
