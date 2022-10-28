#pragma once

#include <Common.hh>

namespace Race {

class Jugem {
public:
    virtual ~Jugem();
    virtual void dt(s32 type);
    virtual void vf_0c();
    virtual void calc();

    void setVisible(bool visible);
    u32 getPlayerId() const;

private:
    u8 _004[0x05e - 0x004];

public:
    bool m_visible; // Added (was padding)

private:
    u8 _05f[0x218 - 0x05f];
};
static_assert(sizeof(Jugem) == 0x218);

} // namespace Race
