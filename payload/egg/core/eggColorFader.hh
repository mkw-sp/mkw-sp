#pragma once

#include <nw4r/ut/Color.hh>

namespace EGG {

class Fader {
protected:
    ~Fader() = default;

public:
    enum class Status {
        Opaque = 0,
        Hidden = 1,
        FadeIn = 2,
        FadeOut = 3,
    };

private:
    virtual void dummy_00() {}
    virtual void dummy_04() {}

public:
    virtual void setStatus(Status status) = 0;
    virtual Status getStatus() const = 0;
    virtual bool fadeIn() = 0;
    virtual bool fadeOut() = 0;
    virtual bool calc() = 0;
    virtual void draw() = 0;
};

class ColorFader : public Fader {
public:
    ColorFader(f32 leftX, f32 bottomY, f32 width, f32 height, nw4r::ut::Color color,
            Fader::Status status);

    void setFrame(u16 frame);
    void setColor(nw4r::ut::Color color);
    void setStatus(Fader::Status status) override;
    Status getStatus() const override;
    bool fadeIn() override;
    bool fadeOut() override;
    bool calc() override;
    void draw() override;
    virtual void dt(s32 type);

private:
    u8 _04[0x24 - 0x04];
};
static_assert(sizeof(ColorFader) == 0x24);

} // namespace EGG
