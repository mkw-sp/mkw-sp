#pragma once

#include <Common.h>
#include <string.h>

typedef struct {
    const char *s;
    size_t len;
} StringView;

#define sv_for_each(sv, itName)                                                  \
    for (const char *itName = sv.s, *const svEnd = sv.s + sv.len;                \
            itName < svEnd &&                                                    \
            (assert(*itName != '\0' && "Null terminator in string view"), true); \
            ++itName)
#define sv_for_each_reversed(sv, itName)                                         \
    for (const char *itName = sv.s + sv.len - 1, *const svStart = sv.s;          \
            itName >= svStart &&                                                 \
            (assert(*itName != '\0' && "Null terminator in string view"), true); \
            --itName)

static inline StringView StringView_create(const char *s) {
    return (StringView){
        .s = s,
        .len = strlen(s),
    };
}
static inline bool StringView_equalsCStr(StringView v, const char *s) {
    const size_t len = strlen(s);
    return v.len == len && !memcmp(v.s, s, len);
}
static inline StringView SubString(StringView view, size_t offset) {
    if (offset > view.len) {
        view.len = 0;
        return view;
    }
    view.s += offset;
    view.len -= offset;
    return view;
}
static inline bool ContainsChar(StringView view, char c) {
    sv_for_each(view, it) {
        if (*it == c) {
            return true;
        }
    }
    return false;
}
static inline StringView SplitLeft(
        StringView view, StringView delim, StringView *remaining) {
    size_t splitOffset = view.len;
    sv_for_each(view, it) {
        if (ContainsChar(delim, *it)) {
            splitOffset = it - view.s;
            break;
        }
    }

    if (remaining != NULL) {
        *remaining = (StringView){
            .s = view.s + splitOffset,
            .len = view.len - splitOffset,
        };
        *remaining = SubString(*remaining, 1);
    }

    return (StringView){
        .s = view.s,
        .len = splitOffset,
    };
}
static StringView SkipLeading(StringView view, char c) {
    sv_for_each(view, it) {
        if (*it == c) {
            ++view.s;
            --view.len;
            continue;
        }
        break;
    }

    return view;
}
static StringView SkipTrailing(StringView view, char c) {
    sv_for_each_reversed(view, it) {
        if (*it == c) {
            --view.len;
            continue;
        }
        break;
    }

    return view;
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
