#include "UIAnimator.h"

bool UIAnimator_isActive(UIAnimator *this, u32 groupId) {
    Group *group = UIAnimator_getGroup(this, groupId);
    return group->isActive;
}

u32 UIAnimator_getAnimation(UIAnimator *this, u32 groupId) {
    Group *group = UIAnimator_getGroup(this, groupId);
    return group->currentAnimation;
}

void UIAnimator_setAnimation(UIAnimator *this, u32 groupId, u32 animationId, f32 frame) {
    Group *group = UIAnimator_getGroup(this, groupId);
    Group_setAnimation(group, animationId, frame);
}

void UIAnimator_setAnimationInactive(UIAnimator *this, u32 groupId, u32 animationId, f32 frame) {
    Group *group = UIAnimator_getGroup(this, groupId);
    Group_setAnimationInactive(group, animationId, frame);
}
