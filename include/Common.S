#pragma once

#define r0 0
#define r1 1
#define r2 2
#define r3 3
#define r4 4
#define r5 5
#define r6 6
#define r7 7
#define r8 8
#define r9 9
#define r10 10
#define r11 11
#define r12 12
#define r13 13
#define r14 14
#define r15 15
#define r16 16
#define r17 17
#define r18 18
#define r19 19
#define r20 20
#define r21 21
#define r22 22
#define r23 23
#define r24 24
#define r25 25
#define r26 26
#define r27 27
#define r28 28
#define r29 29
#define r30 30
#define r31 31

#define f0 0
#define f1 1
#define f2 2
#define f3 3
#define f4 4
#define f5 5
#define f6 6
#define f7 7
#define f8 8
#define f9 9
#define f10 10
#define f11 11
#define f12 12
#define f13 13
#define f14 14
#define f15 15
#define f16 16
#define f17 17
#define f18 18
#define f19 19
#define f20 20
#define f21 21
#define f22 22
#define f23 23
#define f24 24
#define f25 25
#define f26 26
#define f27 27
#define f28 28
#define f29 29
#define f30 30
#define f31 31

#define LR 8

#define SRR0 26
#define SRR1 27

#define IBAT0U 528
#define IBAT0L 529
#define IBAT1U 530
#define IBAT1L 531
#define IBAT2U 532
#define IBAT2L 533
#define IBAT3U 534
#define IBAT3L 535
#define DBAT0U 536
#define DBAT0L 537
#define DBAT1U 538
#define DBAT1L 539
#define DBAT2U 540
#define DBAT2L 541
#define DBAT3U 542
#define DBAT3L 543
#define IBAT4U 560
#define IBAT4L 561
#define IBAT5U 562
#define IBAT5L 563
#define IBAT6U 564
#define IBAT6L 565
#define IBAT7U 566
#define IBAT7L 567
#define DBAT4U 568
#define DBAT4L 569
#define DBAT5U 570
#define DBAT5L 571
#define DBAT6U 572
#define DBAT6L 573
#define DBAT7U 574
#define DBAT7L 575

#define HID0 1008
#define HID4 1011

#define MSR_DR (1 << 4)
#define MSR_IR (1 << 5)

#define BAT_BL_8MB (0b00000111111 << 2)
#define BAT_VS     (1 << 1)
#define BAT_VP     (1 << 0)

#define BAT_PP_RW  (0b10 << 0)

#define PATCH_TYPE_WRITE 0x0
#define PATCH_TYPE_BRANCH 0x1

#define PATCH_REPLACE_START(dst, offset) \
    .global replacement_ ## dst ## _ ## offset; \
    replacement_ ## dst ## _ ## offset:

#define PATCH_REPLACE_END(dst, offset) \
    .size replacement_ ## dst ## _ ## offset, . - replacement_ ## dst ## _ ## offset; \
    .set size_replacement_ ## dst ## _ ## offset, . - replacement_ ## dst ## _ ## offset; \
    .section "patches"; \
    .global patch_ ## dst ## _ ## offset; \
    .size patch_ ## dst ## _ ## offset, 0x10; \
    patch_ ## dst ## _ ## offset: \
        .int PATCH_TYPE_WRITE; \
        .int dst + offset; \
        .int replacement_ ## dst ## _ ## offset; \
        .int size_replacement_ ## dst ## _ ## offset; \
        .int 0; \
    .previous

#define PATCH_BRANCH_START(dst, offset, link) \
    .global dst_ ## dst ## _ ## offset; \
    dst_ ## dst ## _ ## offset:

#define PATCH_BRANCH_END(dst, offset, link) \
    .size dst_ ## dst ## _ ## offset, . - dst_ ## dst ## _ ## offset; \
    .set size_dst_ ## dst ## _ ## offset, . - dst_ ## dst ## _ ## offset; \
    .section "patches"; \
    .global patch_ ## dst ## _ ## offset; \
    .size patch_ ## dst ## _ ## offset, 0x10; \
    patch_ ## dst ## _ ## offset: \
        .int PATCH_TYPE_BRANCH; \
        .int dst + offset; \
        .int dst_ ## dst ## _ ## offset; \
        .byte link; \
        .balign 0x4; \
        .int 0; \
    .previous

#define PATCH_B_START(dst, offset) \
    PATCH_BRANCH_START(dst, offset, 0x0)

#define PATCH_B_END(dst, offset) \
    PATCH_BRANCH_END(dst, offset, 0x0)

#define PATCH_BL_START(dst, offset) \
    PATCH_BRANCH_START(dst, offset, 0x1)

#define PATCH_BL_END(dst, offset) \
    PATCH_BRANCH_END(dst, offset, 0x1)

#define SETTING_RACE 0
#define kSetting_DriftMode SETTING_RACE + 0
#define kSetting_HudLabels SETTING_RACE + 1
#define kSetting_169_Fov SETTING_RACE + 2
#define kSetting_MapIcons SETTING_RACE + 3
#define kSetting_PageTransitions SETTING_RACE + 4
#define kSetting_RaceInputDisplay SETTING_RACE + 5

#define SETTING_TA 6
#define kSetting_TaRuleClass SETTING_TA + 0
#define kSetting_TaRuleGhostSorting SETTING_TA + 1
#define kSetting_TaRuleGhostTagVisibility SETTING_TA + 2
#define kSetting_TaRuleGhostTagContent SETTING_TA + 3
#define kSetting_TaRuleSolidGhosts SETTING_TA + 4
#define kSetting_TaRuleGhostSound SETTING_TA + 5

#define SETTING_LICENSE 12
#define kSetting_MiiAvatar SETTING_LICENSE + 0 
#define kSetting_MiiClient SETTING_LICENSE + 1
#define kSetting_LoadingScreenColor SETTING_LICENSE + 2

#define ChangeGhostData 0x4e