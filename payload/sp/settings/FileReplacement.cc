#include "FileReplacement.hh"

extern "C" {
#include <wchar.h>
}

namespace SP::FileReplacement {

bool IsBRSTMFile(const wchar_t *filepath) {
    const wchar_t brstmFileExtension[] = L".brstm";

    wchar_t *fileExtension = wcsrchr(filepath, L'.');
    if (!fileExtension) {
        return false;
    }

    return wcscmp(fileExtension, brstmFileExtension) == 0;
}

} // namespace SP::FileReplacement
