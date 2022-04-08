#include "CtrlRaceDebugPanel.h"

#include "game/kart/KartObjectManager.h"
#include "game/system/RaceManager.h"

#include <stdarg.h>
#include <wchar.h>

typedef struct {
    u16 offset;
    wchar_t buffer[256];
} WcsWriter;

static void WcsWriter_init(WcsWriter *self) {
    self->offset = 0;
    wmemset(self->buffer, 0, 256);
}

static void WcsWriter_write(WcsWriter *self, const wchar_t *restrict format, ...) {
    if (self->offset >= 256) {
        return;
    }

    va_list args;
    va_start(args, format);
    s32 result = vswprintf(self->buffer + self->offset, 256 - self->offset, format, args);
    va_end(args);

    if (result < 0) {
        self->offset = 256;
    } else {
        self->offset += result;
    }
}

static void WcsWriter_writeVec3(WcsWriter *self, const char *name, const Vec3 *val) {
    WcsWriter_write(self, L"%s %.2f %.2f %.2f\n", name, val->x, val->y, val->z);
}

static void WcsWriter_writeKcl(WcsWriter *self, const CollisionInfo *info) {
    if (info->types & COLLISION_TYPE_FLOOR) {
        u16 val = info->closestFloorSettings << 5 | __builtin_ctz(info->closestFloorFlags);
        WcsWriter_write(self, L" %04x", val);
    } else {
        WcsWriter_write(self, L" N/A");
    }
}

static const LayoutUIControl_vt s_CtrlRaceDebugPanel_vt;

CtrlRaceDebugPanel *CtrlRaceDebugPanel_ct(CtrlRaceDebugPanel *self) {
    LayoutUIControl_ct(self);
    self->vt = &s_CtrlRaceDebugPanel_vt;

    return self;
}

static void CtrlRaceDebugPanel_dt(UIControl *base, s32 type) {
    CtrlRaceDebugPanel *self = (CtrlRaceDebugPanel *)base;

    LayoutUIControl_dt(self, 0);
    if (type > 0) {
        delete(self);
    }
}

static void CtrlRaceDebugPanel_calcSelf(UIControl *base) {
    CtrlRaceDebugPanel *self = (CtrlRaceDebugPanel *)base;

    WcsWriter writer;
    WcsWriter_init(&writer);

    WcsWriter_write(&writer, L"F %u\n", s_raceManager->frame);
    const KartObject *object = s_kartObjectManager->objects[0];
    const Vec3 *pos = KartObjectProxy_getPos(object);
    WcsWriter_writeVec3(&writer, "P", pos);
    const Vec3 *externalVel = &KartObjectProxy_getVehiclePhysics(object)->externalVel;
    WcsWriter_writeVec3(&writer, "EV", externalVel);
    const Vec3 *internalVel = &KartObjectProxy_getVehiclePhysics(object)->internalVel;
    WcsWriter_writeVec3(&writer, "IV", internalVel);
    const Vec3 *movingRoadVel = &KartObjectProxy_getVehiclePhysics(object)->movingRoadVel;
    WcsWriter_writeVec3(&writer, "MRV", movingRoadVel);
    const Vec3 *movingWaterVel = &KartObjectProxy_getVehiclePhysics(object)->movingWaterVel;
    WcsWriter_writeVec3(&writer, "MWV", movingWaterVel);
    const RaceManagerPlayer *player = s_raceManager->players[0];
    WcsWriter_write(&writer, L"CP/KCP/MKCP/JGPT %u %u %u %u\n", player->cp, player->kcp,
        player->maxKcp, player->jgpt);
    WcsWriter_write(&writer, L"LC/RC/MRC %.5f %.5f %.5f\n", player->lapCompletion,
            player->raceCompletion, player->maxRaceCompletion);
    WcsWriter_write(&writer, L"KCL");
    WcsWriter_writeKcl(&writer, KartObjectProxy_getBodyCollisionInfo(object));
    u16 tireCount = KartObjectProxy_getTireCount(object);
    for (u16 i = 0; i < tireCount; i++) {
        WcsWriter_writeKcl(&writer, KartObjectProxy_getTireCollisionInfo(object, i));
    }
    WcsWriter_write(&writer, L"\n");
    const KartMove *move = KartObjectProxy_getKartMove(object);
    WcsWriter_write(&writer, L"SSMT/MT/SMT %u %u %u\n", move->ssmtCharge, move->mtCharge,
            move->smtCharge);
    WcsWriter_write(&writer, L"MTB/MBPB/TZB %u %u %u\n", move->boost.timers[0],
            move->boost.timers[2], move->boost.timers[4]);

    MessageInfo info = {
        .strings[0] = writer.buffer,
    };
    LayoutUIControl_setMessageAll(self, 6602, &info);
}

static const LayoutUIControl_vt s_CtrlRaceDebugPanel_vt = {
    .dt = CtrlRaceDebugPanel_dt,
    .init = LayoutUIControl_init,
    .calc = LayoutUIControl_calc,
    .draw = LayoutUIControl_draw,
    .initSelf = UIControl_initSelf,
    .calcSelf = CtrlRaceDebugPanel_calcSelf,
    .vf_20 = &UIControl_vf_20,
    .vf_24 = &UIControl_vf_24,
    .vf_28 = &LayoutUIControl_vf_28,
    .vf_2c = &LayoutUIControl_vf_2c,
    .vf_30 = &LayoutUIControl_vf_30,
    .vf_34 = &UIControl_vf_34,
    .vf_38 = &LayoutUIControl_vf_38,
};

void CtrlRaceDebugPanel_load(CtrlRaceDebugPanel *self) {
    const char *groups[] = {
        NULL,
        NULL,
    };
    LayoutUIControl_load(self, "game_image", "DebugPanel", "DebugPanel", groups);
}
