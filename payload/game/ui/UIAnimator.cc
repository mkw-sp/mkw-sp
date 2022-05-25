#include "UIAnimator.hh"

namespace UI {

bool UIAnimator::isActive(u32 groupId) {
    return getGroup(groupId)->m_isActive;
}

u32 UIAnimator::getAnimation(u32 groupId) {
    return getGroup(groupId)->m_currentAnimation;
}

void UIAnimator::setAnimation(u32 groupId, u32 animationId, f32 frame) {
    getGroup(groupId)->setAnimation(animationId, frame);
}

void UIAnimator::setAnimationInactive(u32 groupId, u32 animationId, f32 frame) {
    getGroup(groupId)->setAnimationInactive(animationId, frame);
}

} // namespace UI
