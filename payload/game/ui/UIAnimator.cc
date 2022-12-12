#include "UIAnimator.hh"

namespace UI {

f32 Animation::getDuration() const {
    return m_animTransform->getFrameSize();
}

bool Group::getActive() const {
    return m_active;
}

void Group::setActive(bool active) {
    m_active = active;
}

u32 Group::getAnimation() const {
    return m_currentAnimation;
}

f32 Group::getFrame() const {
    return m_frame;
}

f32 Group::getDuration(u32 animationId) const {
    return m_animations[animationId].getDuration();
}

bool UIAnimator::getActive(u32 groupId) const {
    return getGroup(groupId)->getActive();
}

void UIAnimator::setActive(u32 groupId, bool active) {
    getGroup(groupId)->setActive(active);
}

u32 UIAnimator::getAnimation(u32 groupId) const {
    return getGroup(groupId)->getAnimation();
}

f32 UIAnimator::getFrame(u32 groupId) const {
    return getGroup(groupId)->getFrame();
}

f32 UIAnimator::getDuration(u32 groupId, u32 animationId) const {
    return getGroup(groupId)->getDuration(animationId);
}

void UIAnimator::setAnimation(u32 groupId, u32 animationId, f32 frame) {
    getGroup(groupId)->setAnimation(animationId, frame);
}

void UIAnimator::setAnimationInactive(u32 groupId, u32 animationId, f32 frame) {
    getGroup(groupId)->setAnimationInactive(animationId, frame);
}

} // namespace UI
