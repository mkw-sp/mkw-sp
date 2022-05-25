#pragma once

#include <Common.hh>

namespace UI {

class Group {
public:
    void setAnimation(u32 animationId, f32 frame);
    void setAnimationInactive(u32 animationId, f32 frame);

    u8 _00[0x35 - 0x00];
    bool m_isActive;
    u8 _36[0x38 - 0x36];
    u32 m_currentAnimation;
    u8 _3c[0x44 - 0x3c];
};
static_assert(sizeof(Group) == 0x44);

class UIAnimator {
public:
    void dt(s32 type);
    Group *getGroup(u32 groupId);
    
    bool isActive(u32 groupId);
    u32 getAnimation(u32 groupId);
    void setAnimation(u32 groupId, u32 animationId, f32 frame);
    void setAnimationInactive(u32 groupId, u32 animationId, f32 frame);

private:
    u8 _00[0x10 - 0x00];
};
static_assert(sizeof(UIAnimator) == 0x10);

} // namespace UI
