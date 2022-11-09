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
    
    void onRight(u32 localPlayerId, s32 r5);
    void onLeft(u32 localPlayerId, s32 r5);
    void setNumber(u64 number);

private:
    class EditLetter : public LayoutUIControl {
    public:
        u32 m_value;
    };
    static_assert(sizeof(EditLetter) == 0x178);

    u8 _174[0x1dc - 0x174];
    u32 m_digitCount;
    u32 m_position;
    u8 _1e4[0x270 - 0x1e4];
    EditLetter *m_letters;
};
static_assert(sizeof(NumericEditBox) == 0x274);

} // namespace UI
