#define NO_NEW_DELETE

extern "C" {
#include "Keyboard.h"
}

extern "C" {
#include <Common.h>
#include <revolution.h>      // OSReport
#include <revolution/ios.h>  // IOS_Open
}

// Placement new
inline void *operator new(size_t, void *p) {
    return p;
}
#include <string.h>  // memset, memcpy

// #ifdef DEBUG
#define LOG OSReport
// #else
// #define LOG(...)
// #endif

namespace ios {

class handle {
public:
    handle(const char *path) { mHandle = IOS_Open(path, 0); }
    handle() = default;
    handle(const handle &) = delete;  // NonCopyable
    handle(handle &&rhs) {            // Movable
        close();
        mHandle = rhs.mHandle;
        rhs.mHandle = -1;
    }

    ~handle() { close(); }

    s32 ioctl(s32 command, void *source, u32 source_len, void *dest, u32 dest_len) {
        return IOS_Ioctl(mHandle, command, source, source_len, dest, dest_len);
    }

    void close() {
        if (mHandle >= 0)
            IOS_Close(mHandle);
        mHandle = -1;
    }

    s32 mHandle;
};

enum KeyboardMessage { Message_Connect, Message_Disconnect, Message_Press };

struct Modifiers {
    bool right_win : 1;
    bool right_alt : 1;
    bool right_shift : 1;
    bool right_control : 1;
    bool left_win : 1;
    bool left_alt : 1;
    bool left_shift : 1;
    bool left_control : 1;
};

struct KeyboardEvent {
    KeyboardMessage message;  // 4byte
    u32 unknown;              // perhaps keyboard id?
    Modifiers modifiers;
    u8 _09;
    u8 pressed[6];
};

#if 0
void DumpEvent(const KeyboardEvent& ev) {
    static const char* const messages[3] = {
        "Keyboard connect",
        "Keyboard disconnect",
        "Key press"
    };

    OSReport("MessageType: %u %s\n", ev.message, ev.message < 3 ? messages[ev.message] : "INVALID");
    OSReport("Unknown: %u\n", messages[ev.unknown]);
    OSReport("Modifiers:\n");
    if (ev.modifiers.left_control) OSReport("- Left control\n");
    if (ev.modifiers.left_shift) OSReport("- Left shift\n");
    if (ev.modifiers.left_alt) OSReport("- Left alt\n");
    if (ev.modifiers.left_win) OSReport("- Left win\n");
    if (ev.modifiers.right_control) OSReport("- Right control\n");
    if (ev.modifiers.right_shift) OSReport("- Right shift\n");
    if (ev.modifiers.right_alt) OSReport("- Right alt\n");
    if (ev.modifiers.right_win) OSReport("- Right win\n");
    OSReport("_09: %u\n", (unsigned)ev._09);
    OSReport("Pressed:\n");
    for (int i = 0; i < 6; ++i) {
        OSReport("[%u] = %u\n", (unsigned)i, ev.pressed[i]);
    }
}
#endif

constexpr char keys[] = {
    0, 0, 0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
    'z',  // 29
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    0,    // 40, Enter
    0,    // 41, escape
    0,    // 42, backspace
    0,    // 43, tab
    ' ',  // 44 space
    '-',  // 45
    '=',  // 46
    '[', ']',
    '?',   // unknown
    '\\',  // right \ PAD
    ';', '\'', '~', ',', '.', '/',
    0  // CAPS = 57
};
constexpr char keys_shift[] = {
    0, 0, 0, 0, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
    'Z',  // 29
    '!',  // 31 -- This is wrong on wiibrwe, 30
    '@',  // 32
    '#', '$', '%', '^', '&', '*', '(', ')',
    0,    // 40, Enter
    0,    // 41, escape
    0,    // 42, backspace
    0,    // 43, tab
    ' ',  // 44 space
    '_',  // 45
    '+',  // 46
    '{', '}',
    '?',  // unknown
    '|',  // right \ PAD
    ':', '"',
    '~',  // TODO
    '<', '>', '?',
    0  // CAPS = 57
};

constexpr bool key_is_enter(u32 key_code) {
    return key_code == 40 /* ENTER */;
}

constexpr bool key_is_backspace(u32 key_code) {
    return key_code == 42 /* BACKSPACE */;
}

constexpr bool key_is_character(u32 key) {
    return key < sizeof(keys) && keys[key] != 0;
}
constexpr char key_as_character(u32 key, bool shift) {
    const char *table = (shift) ? &keys_shift[0] : &keys[0];

    return table[key];
}

class keyboard {
    ios::handle mHandle;

public:
    keyboard() : mHandle("/dev/usb/kbd") {
        if (mHandle.mHandle < 0) {
            OSReport("Failed to open /dev/usb/kbd: Returned %i\n", mHandle.mHandle);
        } else {
            OSReport("Success: ID: %i\n", mHandle.mHandle);
        }
    }

    s32 pollBlocking(ios::KeyboardEvent &ev) {
        // No request type?
        memset(&ev, 0, sizeof(ios::KeyboardEvent));

        return mHandle.ioctl(0, nullptr, 0, &ev, sizeof(ios::KeyboardEvent));
    }

    bool next(ios::KeyboardEvent &event) {
        pollBlocking(event);
        return event.message != 0;
    }
};

}  // namespace ios

// struct KeyReceiver {
//   void onEnterPressed();
//   void onBackspacePressed();
//   void onCharacterPressed(char c);
// };

namespace detail {
template <typename T>
void processEvents(T &receiver, ios::keyboard &keyboard);
}  // namespace detail

template <typename T>
void ConsumeIosKeyboardEvents(T &receiver, ios::keyboard &keyboard) {
    if (false) {
        // A receiver must implement these methods!
        receiver.onEnterPressed();
        receiver.onBackspacePressed();
        receiver.onCharacterPressed('A');
    }

    detail::processEvents(receiver, keyboard);
}

//
// Implementation for ConsumeIosKeyboardEvents
//
namespace detail {

//
// Key-Code Level
//

template <typename T>
void onKeyCodePressed(T &receiver, u32 key_code, bool shift_pressed) {
    if (ios::key_is_enter(key_code)) {
        receiver.onEnterPressed();
        return;
    }

    if (ios::key_is_backspace(key_code)) {
        receiver.onBackspacePressed();
        return;
    }

    if (ios::key_is_character(key_code)) {
        receiver.onCharacterPressed(ios::key_as_character(key_code, shift_pressed));
    }
}

//
// Event Level
//

template <typename T>
void dispatchEvent(T &receiver, const ios::KeyboardEvent &ev) {
    if (ev.message != ios::Message_Press)
        return;

    const bool shift_pressed = ev.modifiers.left_shift | ev.modifiers.right_shift;

    for (int i = 0; i < 6; ++i) {
        const u32 pressed = ev.pressed[i];
        if (pressed == 0)
            continue;

        onKeyCodePressed(receiver, pressed, shift_pressed);
    }
}

template <typename T>
void processEvents(T &receiver, ios::keyboard &keyboard) {
    // NOTE: No thread contention here
    // MultiThreaded g;

    // Dolphin can detect up background input if configured to do so
    // Mashing the keyboard will rarely yield 1, extra rarely 2 events
    // But background typing can be in the hundreds
    // Let's only tolerate 2 buffered events
    enum { MAX_BUFFERED_EVENTS = 2 };

    ios::KeyboardEvent events[MAX_BUFFERED_EVENTS];

    int i = 0;
    while (keyboard.next(events[i])) {
        // OSReport("Event %i\n", i);
        if (i >= MAX_BUFFERED_EVENTS) {
            // - Discard the previous two events
            // - Discard all future events clearing the stack
            LOG("Detected background input:\n");

            while (keyboard.next(events[0]) && i < 5) {
                ++i;
            }

            LOG("-> %i keyboard packets; %i is max\n", i, MAX_BUFFERED_EVENTS);

            return;
        }

        ++i;
    }

    for (int j = 0; j < i; ++j) {
        dispatchEvent(receiver, events[j]);
    }
}

}  // namespace detail

//! Primitive implementation of a RingBuffer; acts as a regular array, shuffling
//! memory as needed
//!
template <typename T, int Capacity>
class RingBuffer {
public:
    inline RingBuffer() : mSize(0) {}

    inline void reset() { mSize = 0; }

    inline void zeroset() {
        mSize = 0;
        memset(&mBuf[0], 0, sizeof(T) * Capacity);
    }

    inline void clear() { zeroset(); }

    inline void resize(int new_size) { mSize = new_size; }
    // new_size < mSize
    inline void shrink_and_zeroset(int new_size) {
        memset(&mBuf[new_size - 1], 0, sizeof(T) * (mSize - new_size));
        mSize = new_size;
    }
    inline int size() const { return mSize; }
    inline bool empty() const { return mSize == 0; }

    inline T *data() { return &mBuf[0]; }
    inline const T *data() const { return &mBuf[0]; }

    // index < mSize
    inline const T &at(int index) const { return mBuf[index]; }
    inline const T &operator[](int index) const { return mBuf[index]; }
    inline T &operator[](int index) { return mBuf[index]; }

    // If the capacity is reached, the buffer starts again at 1
    T &emplace_back(const T &vec) {
        if (mSize >= Capacity) {
            mSize = Capacity - 1;
            memmove(&mBuf[0], &mBuf[1], (Capacity - 1) * sizeof(T));
        }

        return mBuf[mSize++] = vec;
    }

    // If the capacity is reached, the buffer starts again at 1
    T &emplace_empty() {
        if (mSize >= Capacity) {
            mSize = Capacity - 1;
            memmove(&mBuf[0], &mBuf[1], (Capacity - 1) * sizeof(T));
        }

        return mBuf[mSize++];
    }

private:
    int mSize;
    T mBuf[Capacity];
};

#define NUM_CONS_LINES 21

// Requires T:
// - void T::acceptLine(const char* buf, u32 len);
// - bool T::filterCharacter(char c);
//
// Models KeyReceiver (onEnterPressed, onBackspacePressed, onCharacterPressed)
//
template <typename T>
class KeyboardCharacterBuffer {
public:
    //
    // User Level
    //

    void onEnterPressed() {
        if (!getDerived().filterCharacter(0xFF))
            return;

        getDerived().acceptLine(mBuf.data(), mBuf.size());
        mBuf.zeroset();
    }

    void onBackspacePressed() {
        if (!getDerived().filterCharacter(0xFF))
            return;

        if (mBuf.size() != 0) {
            mBuf[mBuf.size() - 1] = 0;
            mBuf.resize(mBuf.size() - 1);
        }
    }

    void onCharacterPressed(char c) {
        if (getDerived().filterCharacter(c))
            mBuf.emplace_back(c);
    }

    void processEvents() { ConsumeIosKeyboardEvents(*this, mKeyboard); }

    RingBuffer<char, 64> mBuf;

    ios::keyboard mKeyboard;

    T &getDerived() { return *static_cast<T *>(this); }
};

namespace sp {

enum ConsoleState {
    kConsoleState_Default = 0,
    kConsoleState_Typing = (1 << 0),
};

class ConsoleInput : public KeyboardCharacterBuffer<ConsoleInput> {
public:
    // Implements KeyboardCharacterBuffer::acceptLine
    void acceptLine(const char *buf, u32 len) {
        if (mpCallback != nullptr)
            (*mpCallback)(buf, static_cast<size_t>(len));

        // End the console interaction
        mConsoleState &= ~kConsoleState_Typing;
    }

    // Implements KeyboardCharacterBuffer::filterCharacter
    bool filterCharacter(char c) {
        // If we have already started a console interaction, continue it
        if (mConsoleState & kConsoleState_Typing)
            return true;

        // Trigger a console interaction
        if (c == '/') {
            mConsoleState |= kConsoleState_Typing;
            return true;
        }

        return false;
    }

    void calc() { KeyboardCharacterBuffer::processEvents(); }

    void setCallback(OnLineCallback cb) { mpCallback = cb; }

private:
    u32 mConsoleState = kConsoleState_Default;
    OnLineCallback mpCallback = nullptr;
};

// Avoids static initialization
template <typename T>
struct DeferredInitialization {
    u8 data[sizeof(T)]{ 0 };

    u8 initd{ 0 };  // This might cause issues
    inline bool isInitialized() const { return initd; }
    template <typename... Args>
    inline void initialize(Args... args) {
        new ((void *)&data) T(args...);
        initd = 1;
    }
    inline void deinitialize() {
        ((T &)*this).~T();
        initd = 0;
    }

    inline operator T &() { return *reinterpret_cast<T *>(&data); }
    inline operator const T &() const { return *reinterpret_cast<const T *>(&data); }

    inline T &operator*() { return *reinterpret_cast<T *>(&data); }
    inline const T &operator*() const { return *reinterpret_cast<const T *>(&data); }

    inline T *operator->() { return reinterpret_cast<T *>(&data); }
    inline const T *operator->() const { return reinterpret_cast<T *>(&data); }
};

DeferredInitialization<ConsoleInput> sConsoleInput;

}  // namespace sp

extern "C" void SP_InitConsoleInput(void) {
    if (!sp::sConsoleInput.isInitialized()) {
        sp::sConsoleInput.initialize();
    }
}

extern "C" bool SP_IsConsoleInputInit(void) {
    return sp::sConsoleInput.isInitialized();
}

extern "C" void SP_DestroyConsoleInput(void) {
    sp::sConsoleInput.deinitialize();
}

extern "C" void SP_ProcessConsoleInput(void) {
    assert(sp::sConsoleInput.isInitialized());
    sp::sConsoleInput->calc();
}

extern "C" void SP_SetLineCallback(OnLineCallback callback) {
    assert(sp::sConsoleInput.isInitialized());
    sp::sConsoleInput->setCallback(callback);
}