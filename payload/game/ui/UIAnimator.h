#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x35 - 0x00];
    bool isActive;
    u8 _36[0x38 - 0x36];
    u32 currentAnimation;
    u8 _3c[0x44 - 0x3c];
} Group;
static_assert(sizeof(Group) == 0x44);

void Group_setAnimation(Group *this, u32 animationId, f32 frame);

void Group_setAnimationInactive(Group *this, u32 animationId, f32 frame);

typedef struct {
    u8 _00[0x10 - 0x00];
} UIAnimator;
static_assert(sizeof(UIAnimator) == 0x10);

Group *UIAnimator_getGroup(UIAnimator *this, u32 groupId);

bool UIAnimator_isActive(UIAnimator *this, u32 groupId);

u32 UIAnimator_getAnimation(UIAnimator *this, u32 groupId);

void UIAnimator_setAnimation(UIAnimator *this, u32 groupId, u32 animationId, f32 frame);

void UIAnimator_setAnimationInactive(UIAnimator *this, u32 groupId, u32 animationId, f32 frame);
