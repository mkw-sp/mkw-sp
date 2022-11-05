#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class NumericEditBox : public LayoutUIControl {
public:
    NumericEditBox();
    ~NumericEditBox() override;

    void load(u32 digitCount, const char *dir, const char *boxFile, const char *boxVariant,
            const char *digitFile, const char *digitVariant, u32 playerFlags, bool stack_b,
            bool pointerOnly);
    void reset();
    void insert(u32 digit);
    void remove();
    bool isEmpty();
    bool isFull();
    u64 getNumber();

private:
    u8 _174[0x274 - 0x174];
};
static_assert(sizeof(NumericEditBox) == 0x274);

} // namespace UI
