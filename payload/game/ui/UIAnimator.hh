#pragma once

#include <nw4r/lyt/lyt_animTransform.hh>

namespace UI {

class Animation {
public:
    f32 getDuration() const;

private:
    nw4r::lyt::AnimTransform *m_animTransform;
    u8 _04[0x10 - 0x04];
};
static_assert(sizeof(Animation) == 0x10);

class Group {
public:
    bool getActive() const;
    void setActive(bool active);
    u32 getAnimation() const;
    f32 getFrame() const;
    f32 getDuration(u32 animationId) const;
    void setAnimation(u32 animationId, f32 frame);
    void setAnimationInactive(u32 animationId, f32 frame);

private:
    Animation *m_animations;
    u8 _04[0x35 - 0x04];
    bool m_active;
    u8 _36[0x38 - 0x36];
    u32 m_currentAnimation;
    f32 m_frame;
    u8 _40[0x44 - 0x40];
};
static_assert(sizeof(Group) == 0x44);

class UIAnimator {
public:
    UIAnimator();
    void dt(s32 type);

    const Group *getGroup(u32 groupId) const;
    Group *getGroup(u32 groupId);    
    bool getActive(u32 groupId) const;
    void setActive(u32 groupId, bool active);
    u32 getAnimation(u32 groupId) const;
    f32 getFrame(u32 groupId) const;
    f32 getDuration(u32 groupId, u32 animationId) const;
    void setAnimation(u32 groupId, u32 animationId, f32 frame);
    void setAnimationInactive(u32 groupId, u32 animationId, f32 frame);

private:
    u8 _00[0x10 - 0x00];
};
static_assert(sizeof(UIAnimator) == 0x10);

} // namespace UI
