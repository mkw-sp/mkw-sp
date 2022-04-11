extern "C" {
#include <sp/Host.h>
#include <sp/IniReader.h>
#include <sp/Net.h>
#include <sp/NetStorageClient.h>
#include <sp/Yaz.h>
}

#include <chrono>
#include <utility>
#include <vector>

template <typename TFunctor>
struct Defer {
    TFunctor mF;
    Defer(TFunctor &&F) : mF(std::move(F)) {}
    ~Defer() { mF(); }
};

template <typename TFunctor>
static auto MakeDefer(TFunctor &&F) {
    return Defer<TFunctor>(std::move(F));
}
#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#define SP_DEFER(f) const auto MACRO_CONCAT(__tmp, __COUNTER__) = MakeDefer([&]() { f; })

static void NetStorageClient_Test() {
    NetStorageClient client;
    NetStorageClient_create(&client);
    SP_DEFER(NetStorageClient_destroy(&client));

    if (!NetStorageClient_connect(&client, 127, 0, 0, 1, 1234)) {
        SP_LOG("Failed to connect");
        return;
    }
    SP_DEFER(NetStorageClient_disconnect(&client));

    {
        char buf[128];
        snprintf(buf, sizeof(buf), R"("IdentifyV1":{"cc":"%s"})", __VERSION__);
        if (NetStorage_sendJSONCommands(&client, buf)) {
            char response[256];
            TcpSocket_receiveBytes(&client.sock, response, sizeof(response));
            response[sizeof(response) - 1] = 0;
            SP_LOG("Response: %s", response);
        }
    }

    {
        NetDir d;
        memset(&d, 0, sizeof(d));

        if (!NetDir_open(&d, &client, L"/mkw-sp/disc/Race/Course")) {
            SP_LOG("Failed to open directory");
            return;
        }
        SP_DEFER(NetDir_close(&d));

        SP_LOG("Courses:");

        NetDirEntry cur;
        while (NetDir_read(&d, &cur)) {
            SP_LOG("- %ls (%s)", cur.name, cur.isDir ? "DIR" : "FILE");
        }
    }

    NetFile f;
    NetFile_create(&f);
    if (!NetFile_open(&f, &client, L"/mkw-sp/disc/Race/Course/castle_course.szs")) {
        SP_LOG("Failed to open");
        return;
    }
    SP_DEFER(NetFile_close(&f));

    // Print magic
    {
        char buf[4];
        NetFile_read(&f, buf, sizeof(buf), 0);

        // Expected: Yaz0
        SP_LOG("BUF: %c%c%c%c", buf[0], buf[1], buf[2], buf[3]);
    }

    u8 header[8];
    NetFile_read(&f, header, sizeof(header), 0);

    u32 encodedSize = f.node.fileSize;
    SP_LOG("Encoded size: %u", encodedSize);

    u32 decodedSize = Yaz_getSize(header);
    SP_LOG("Decoded size: %u", decodedSize);

    std::vector<u8> encoded(encodedSize);
    NetFile_read(&f, encoded.data(), encodedSize, 0);

    std::vector<u8> decoded(decodedSize);
    Yaz_decode(encoded.data(), decoded.data(), encodedSize, decodedSize);

    // Expected: U¬8-
    SP_LOG("BUF: %c%c%c%c", (char)decoded[0], (char)decoded[1], (char)decoded[2],
            (char)decoded[3]);

    {
        SP_LOG("Compressing...");

        std::vector<u8> compressed(decodedSize);
        {
            const auto begin = std::chrono::steady_clock::now();

            u32 dstSize = Yaz_encode(
                    decoded.data(), compressed.data(), decoded.size(), compressed.size());
            SP_LOG("Compressed to %u bytes", dstSize);

            const auto end = std::chrono::steady_clock::now();

            unsigned ms =
                    std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
                            .count();
            SP_LOG("(Took %u ms)", ms);
        }

        SP_LOG("Decompressing...");
        std::vector<u8> decompressed(Yaz_getSize(compressed.data()));
        Yaz_decode(compressed.data(), decompressed.data(), compressed.size(),
                decompressed.size());

        SP_LOG("Checking...");
        const bool isValid = decompressed == decoded;

        SP_LOG("Recompression worked? %s", isValid ? "YES" : "NO");
    }
}

// Get a stack-allocated CString of a string view
#define sv_as_cstr(sv, svLen)                             \
    ({                                                    \
        char *cstr = (char *)__builtin_alloca(svLen + 1); \
        const size_t lenWritten = MIN(svLen, sv.len);     \
        memcpy(cstr, sv.s, lenWritten);                   \
        cstr[lenWritten] = '\0';                          \
        cstr;                                             \
    })

static void IniTest() {
    const char *iniFile =
            "# Settings\n"
            "\n"
            "  [Race] # This is a section \n"
            "  DriftMode =  Manual # Manual / Auto \n"
            "  HudLabels =  Show   ; Show / Hide   \n"
            "\n"
            "[Ghost]\n"
            "AllowVis=True\n";
    IniRange iniRange = IniRange_create(iniFile, strlen(iniFile));

    IniProperty prop;
    while (IniRange_next(&iniRange, &prop)) {
        const char *section = sv_as_cstr(prop.section, 64);
        const char *key = sv_as_cstr(prop.key, 64);
        const char *value = sv_as_cstr(prop.value, 64);

        SP_LOG("[%s] %s:%s", section, key, value);
    }
}

int main() {
    Host_Init();

    // Windows requires WSAStartup before sockets can be used.
    if (!Net_init()) {
        SP_LOG("Failed to initialize networking");
        return 1;
    }

    // Example output:
    //     --------------------------------
    //     MKW-SP v0.1.4 (Release) REV <GIT ?>
    //     Region: PAL, System: Windows
    //     Built Mar  7 2022 at 02:57:14, Clang 12.0.0
    // --------------------------------
    Host_PrintMkwSpInfo(OSReport);

    IniTest();

    NetStorageClient_Test();

    return 0;
}
